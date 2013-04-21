#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "Xsocket.h"
#include "dagaddr.hpp"
#include "ChunkFetcher.h"
#include "ChunkQueue.h"
#include "Chunk.h"
#include "Utility.h"

const int CHUNK_QUEUE_THRESHOLD_SIZE = 30;

// Thread will continually check the queue to see if it needs to fetch more chunks
const double NUM_SECONDS_TO_WAIT_BETWEEN_QUEUE_THRESHOLD_CHECKING = 1.0;

const double NUM_SECONDS_TO_WAIT_FOR_NEXT_CHUNK = 0.0;

using namespace std;

ChunkFetcher::ChunkFetcher(int xSocket_, int numChunksInFile_, const char* serverAd, const char* serverHid):
    xSocket(xSocket_),
    numChunksInFile(numChunksInFile_),
    nextChunkToRequest(0),
    reachedEndOfFile(false),
    SERVER_AD(serverAd),
    SERVER_HID(serverHid)
{
    // Create chunk socket
    // We will use this to receive chunks
    if ((chunkSock = Xsocket(AF_XIA, XSOCK_CHUNK, 0)) < 0){
        cerr << "unable to create chunk socket" << endl;
        exit(-1);
    }

    chunkQueue = new ChunkQueue();
}

ChunkFetcher::~ChunkFetcher(){
    delete chunkQueue;
}

Chunk* ChunkFetcher::getNextChunkFromQueue(){
    if(reachedEndOfFile)
        return NULL;
        
    // Otherwise, idle around and wait for chunks to get fetched
    while(chunkQueue->size() == 0){
        sleep(NUM_SECONDS_TO_WAIT_FOR_NEXT_CHUNK);
    }
    
    Chunk* chunkToReturn = chunkQueue->front();
    chunkQueue->pop();
    return chunkToReturn;
}


void* ChunkFetcher::fetchChunks(void* chunkFetcher_){
    ChunkFetcher* chunkFetcher = static_cast<ChunkFetcher* >(chunkFetcher_);

    while( !chunkFetcher->reachedEndOfFile ){
        while(chunkFetcher->chunkQueue->size() < CHUNK_QUEUE_THRESHOLD_SIZE){
            chunkFetcher->fetchChunkWindow();
        }
        
        sleep(  NUM_SECONDS_TO_WAIT_BETWEEN_QUEUE_THRESHOLD_CHECKING);
    }
}


void ChunkFetcher::fetchChunkWindow(){
    char* listOfChunkCIDs = retrieveCIDs();
    if(listOfChunkCIDs){
        readChunkData(listOfChunkCIDs);
        delete listOfChunkCIDs;
    }
}


char* ChunkFetcher::retrieveCIDs(){
    if(numChunksInFile <= nextChunkToRequest){
        reachedEndOfFile = true;
	    return NULL;
    }


    // GET LIST OF CIDs FROM SERVER
    // Determine how many chunks to ask for
    int numToReceive = CHUNK_WINDOW_SIZE;
    if (numChunksInFile - nextChunkToRequest < numToReceive){
        numToReceive = numChunksInFile - nextChunkToRequest;
    }

    // tell the server we want a list of <numToReceive> cids starting at location <offset>
    // block LHS:RHS [LHS, RHS) -- RHS is one past the range we want
    char cmd[512];
    sprintf(cmd, "block %d:%d", nextChunkToRequest, nextChunkToRequest + numToReceive);
    cout << "Sending Chunk Request: " << cmd << endl;
    sendCmd(cmd);

    // Server replies with "CID:" followed by a list of CIDs
    char reply[REPLY_MAX_SIZE];
    receiveReply(reply, sizeof(reply));
    cout << "Received Reply: " << reply << endl;

    nextChunkToRequest += CHUNK_WINDOW_SIZE;

    char* cid_list = new char[REPLY_MAX_SIZE];
    strncpy(cid_list, reply, REPLY_MAX_SIZE);

    return cid_list;
}



int ChunkFetcher::readChunkData(char* listOfChunkCIDs){
    ChunkStatus chunkStatuses[CHUNK_WINDOW_SIZE];
    char *chunk_ptr = listOfChunkCIDs;

    // Number of chunks in the CID List that we assemble
    int numChunks = 0;

    cout << "Getting File Data" << endl;
    // build the list of chunk CID chunkStatuses (including Dags) to retrieve
    char* next = NULL;
    while ((next = strchr(chunk_ptr, ' '))) {
        *next = 0;

        char* dag = (char *)malloc(512);
        sprintf(dag, "RE ( %s %s ) %s", SERVER_AD, SERVER_HID, chunk_ptr);
        //printf("getting %s\n", chunk_ptr);
        chunkStatuses[numChunks].cidLen = strlen(dag);
        chunkStatuses[numChunks].cid = dag;
        numChunks++;

        // Set chunk_ptr to point to the next position (following the space)
        chunk_ptr = next + 1;
    }

    cout << "numChunks: " << numChunks << endl;

    // Add the last chunk CID onto the end of the CID chunkStatus list
    // Commented this out since it was causing 11 things to get requested when should be 10
    // {
    //     char* dag = (char *) malloc(512);
    //     sprintf(dag, "RE ( %s %s ) CID:%s", SERVER_AD, SERVER_HID, chunk_ptr);
    //     //printf("getting %s\n", chunk_ptr);
    //     chunkStatuses[numChunks].cidLen = strlen(dag);
    //     chunkStatuses[numChunks].cid = dag;
    //     numChunks++;
    // }


    // BRING LIST OF CHUNKS LOCAL
    cout << "requesting list of " << numChunks << " chunks" << endl;
    if (XrequestChunks(chunkSock, chunkStatuses, numChunks) < 0) {
        cerr << "unable to request chunks" << endl;
        return -1;
    }


    // IDLE AROUND UNTIL ALL CHUNKS ARE READY
    cout << "checking chunk status\n";
    while (1) {
        int status = XgetChunkStatuses(chunkSock, chunkStatuses, numChunks);
        //printChunkStatuses(chunkStatuses, numChunks);

        if (status == READY_TO_READ){
            break;
        }
        else if (status < 0) { // REQUEST_FAILED Or INVALID_HASH
            cout << "error getting chunk status" << endl;
            return -1;

        } else if (status == WAITING_FOR_CHUNK) {
            // one or more chunks aren't ready.
            cout << "waiting... one or more chunks aren't ready yet" << endl;
        }
        sleep(1);
    }

    cout << "all chunks ready\n";


    // RECEIVE EACH CHUNK
    for (int i = 0; i < numChunks; i++) {
        char *cid = strrchr(chunkStatuses[i].cid, ':');
        cid++;
        cout << "reading chunk " << cid << endl;

        char chunkData[XIA_MAXCHUNK];
        memset(chunkData, 0, XIA_MAXCHUNK);

        // Receive the chunk, and write into data buffer
        int len = 0;
        if ((len = XreadChunk(chunkSock, chunkData, XIA_MAXCHUNK, 0, chunkStatuses[i].cid, chunkStatuses[i].cidLen)) < 0) {
            cout << "error getting chunk\n";
            return -1;
        }
        cout << "len: " << len << endl;

        chunkQueue->push(new Chunk(chunkData, len));

        free(chunkStatuses[i].cid);
        chunkStatuses[i].cid = NULL;
        chunkStatuses[i].cidLen = 0;
    }

    return numChunks;
}


int ChunkFetcher::sendCmd(const char *cmd)
{
    int n;

    if ((n = Xsend(xSocket, cmd,  strlen(cmd), 0)) < 0) {
        Xclose(xSocket);
        cerr << "Unable to communicate with the server\n";
        exit(-1);
    }

    return n;
}


int ChunkFetcher::receiveReply(char *reply, int size)
{
    int n;

    // Receive (up to) size bytes from the socket, write to reply
    if ((n = Xrecv(xSocket, reply, size, 0))  < 0) {
        Xclose(xSocket);
        cerr << "Unable to communicate with the server" << endl;
        exit(-1);
    }

    // If the first 3 characters were not "OK:", die
    // if (strncmp(reply, "OK:", 3) != 0) {
    //     die(-1, "%s\n", reply);
    // }

    //Append null character
    reply[n] = 0;

    // Return number of bytes successfully received
    return n;
}


// Byte Counter Function Object
// Used as a helper for numBytesReady() to count the bytes
// present in the queue
struct ByteCounterFO{
    int count;
    ByteCounterFO():count(0) {}
    void operator()(Chunk* chunk){
        count += chunk->size();
    }
};

/*
int ChunkFetcher::numBytesReady(){
    // num bytes ready is bytes remaining from current chunk + 
    // num bytes in the chunk queue
    int numBytesReady = 0;
    ByteCounterFO byteCounter;
    byteCounter = for_each(chunkQueue.begin(), chunkQueue.end(), byteCounter);
    
    numBytesReady += byteCounter.count;
    
    if (currentChunk){
        numBytesReady += currentChunk->size() - numBytesReadFromCurrentChunk;
    }
    
    return numBytesReady;
}
*/