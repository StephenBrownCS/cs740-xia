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
#include "ClientConfig.h"
#include "VideoInformation.h"

// Maximum length that hte CID DAG may be for any given chunk
// May need to be lengthened with increasingly complex 
// fallback routes
const int MAX_LENGTH_OF_CID_DAG = 512;

using namespace std;

ChunkFetcher::ChunkFetcher(int xSocket_, VideoInformation & videoInformation_):
    xSocket(xSocket_),
    videoInformation(videoInformation_),
    nextChunkToRequest(0),
    reachedEndOfFile(false)
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
    Xclose(chunkSock);
}

Chunk* ChunkFetcher::getNextChunkFromQueue(){
    if(reachedEndOfFile)
        return NULL;
        
    // Otherwise, idle around and wait for chunks to get fetched
    while(chunkQueue->size() == 0 && !reachedEndOfFile){
        cout << "Waiting for Queue to get refilled" << endl;
        thread_sleep(NUM_SECONDS_TO_WAIT_FOR_NEXT_CHUNK);
    }
    
    if(reachedEndOfFile)
        return NULL;

    Chunk* chunkToReturn = chunkQueue->front();
    chunkQueue->pop();
    return chunkToReturn;
}


void* ChunkFetcher::fetchChunks(void* chunkFetcher_){
    ChunkFetcher* chunkFetcher = static_cast<ChunkFetcher* >(chunkFetcher_);

    while( !chunkFetcher->reachedEndOfFile ){
        while(chunkFetcher->chunkQueue->size() < CHUNK_QUEUE_THRESHOLD_SIZE && 
              !chunkFetcher->reachedEndOfFile){
            cout << "Fetching Chunk Window" << endl;
            chunkFetcher->fetchChunkWindow();
        }
        
        cout << "Sleeping before we check queue size again" << endl;
        thread_sleep(  NUM_SECONDS_TO_WAIT_BETWEEN_QUEUE_THRESHOLD_CHECKING);
    }

    cout << "All chunks fetched" << endl;
    cout << "Terminating Chunk Fetcher Thread" << endl;

    return NULL;
}


void ChunkFetcher::fetchChunkWindow(){
    char* listOfChunkCIDs = retrieveCIDs();
    if(listOfChunkCIDs){
        readChunkData(listOfChunkCIDs);
        delete listOfChunkCIDs;
    }
}


char* ChunkFetcher::retrieveCIDs(){
    if(videoInformation.numChunks <= nextChunkToRequest){
        reachedEndOfFile = true;
        return NULL;
    }


    // GET LIST OF CIDs FROM SERVER
    // Determine how many chunks to ask for
    int numToReceive = CHUNK_WINDOW_SIZE;
    if (videoInformation.numChunks - nextChunkToRequest < numToReceive){
        numToReceive = videoInformation.numChunks - nextChunkToRequest;
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

    // build the list of chunk CID chunkStatuses (including Dags) to retrieve
    char* next = NULL;
    while ((next = strchr(chunk_ptr, ' '))) {
        *next = 0;

        // Create DAG for CID
        cout << "Creating dag" << endl;
        char* dag = (char *)malloc(MAX_LENGTH_OF_CID_DAG);
        
        /*
        Node n_src;
        Node n_ad(Node::XID_TYPE_AD, "1000000000000000000000000000000000000000");
        Node n_hid(Node::XID_TYPE_HID, "0000000000000000000000000000000000000000");
        Node n_cid(Node::XID_TYPE_CID, chunk_ptr);
        Graph g3 = n_src * n_ad * n_hid * n_cid;
        
        Node n_ad2(Node::XID_TYPE_AD, "2000000000000000000000000000000000000000");
        Node n_hid2(Node::XID_TYPE_HID, "2000000000000000000000000000000000000002");
        Graph g2 = n_src * n_ad2 * n_hid2 * n_cid;
        
        //Graph g3 = g1;// + g2;
        
        cout << g3.dag_string() << endl;
        
        const char* g_ptr = g3.dag_string().c_str();
        char* dag_ptr = dag;
        for(int i = 0; i < g3.dag_string().length(); i++){
            *dag_ptr++ = *g_ptr++;
        }
        */
        
        cout << "It's Jon!" << endl;
        
        ServerLocation serverLocation = videoInformation.getServerLocation(0);
        sprintf(dag, "RE ( AD:%s HID:%s ) %s", serverLocation.getAd().c_str(), serverLocation.getHid().c_str(), chunk_ptr);
        cout << dag << endl;
        printf("getting %s\n", chunk_ptr);
        chunkStatuses[numChunks].cidLen = strlen(dag);
        chunkStatuses[numChunks].cid = dag;
        //chunkStatuses[numChunks].cidLen = g3.dag_string().length();
        //chunkStatuses[numChunks].cid = dag;
        numChunks++;

        // Set chunk_ptr to point to the next position (following the space)
        chunk_ptr = next + 1;
    }

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

    cout << "Bring local" << endl;

    // BRING LIST OF CHUNKS LOCAL
    if (XrequestChunks(chunkSock, chunkStatuses, numChunks) < 0) {
        cerr << "unable to request chunks" << endl;
        return -1;
    }


    // IDLE AROUND UNTIL ALL CHUNKS ARE READY
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

    cout << "All Chunks Ready " << endl;

    // RECEIVE EACH CHUNK
    for (int i = 0; i < numChunks; i++) {
        char *cid = strrchr(chunkStatuses[i].cid, ':');
        cid++;

        char chunkData[XIA_MAXCHUNK];
        memset(chunkData, 0, XIA_MAXCHUNK);

        // Receive the chunk, and write into data buffer
        int len = 0;
        if ((len = XreadChunk(chunkSock, chunkData, XIA_MAXCHUNK, 0, chunkStatuses[i].cid, chunkStatuses[i].cidLen)) < 0) {
            cout << "error getting chunk\n";
            return -1;
        }

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
