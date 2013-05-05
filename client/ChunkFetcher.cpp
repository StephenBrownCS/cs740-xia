#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <sstream>
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

// Maximum length that the CID DAG may be for any given chunk
// May need to be lengthened with increasingly complex 
// fallback routes
const int MAX_LENGTH_OF_CID_DAG = 512;

using namespace std;


/**
 * Useful for debugging: prints the CID and status of each chunkStatus
 *
*/
void printChunkStatuses(ChunkStatus* chunkStatuses, int numChunks);


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
        say("Waiting for Queue to get refilled", LVL_DEBUG);
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
            say("Fetching Chunk Window", LVL_DEBUG);
            chunkFetcher->fetchChunkWindow();
        }
        
        say("Sleeping before we check queue size again", LVL_DEBUG);
        thread_sleep(  NUM_SECONDS_TO_WAIT_BETWEEN_QUEUE_THRESHOLD_CHECKING);
    }

    say("All chunks fetched", LVL_INFO);
    say("Terminating Chunk Fetcher Thread", LVL_INFO);

    return NULL;
}


void ChunkFetcher::fetchChunkWindow(){
    say("Retrieving CIDs...", LVL_DEBUG);
    vector<string> listOfChunkCIDs = retrieveCIDs();
    say("done!", LVL_DEBUG);
    if(!listOfChunkCIDs.empty()){
        readChunkData(listOfChunkCIDs);
    }
}


vector<string> ChunkFetcher::retrieveCIDs(){
    vector<string> cidList;
    if(videoInformation.getNumChunks() <= nextChunkToRequest){
        reachedEndOfFile = true;
        return cidList;
    }


    // GET LIST OF CIDs FROM SERVER
    // Determine how many chunks to ask for
    int numToReceive = CHUNK_WINDOW_SIZE;
    if (videoInformation.getNumChunks() - nextChunkToRequest < numToReceive){
        numToReceive = videoInformation.getNumChunks() - nextChunkToRequest;
    }

    // tell the server we want a list of <numToReceive> cids starting at location <offset>
    // block LHS:RHS [LHS, RHS) -- RHS is one past the range we want
    char cmd[512];
    sprintf(cmd, "block %d:%d", nextChunkToRequest, nextChunkToRequest + numToReceive);
    
    sendCmd(cmd);

    // Server replies with "CID:" followed by a list of CIDs
    char reply[REPLY_MAX_SIZE];
    receiveReply(reply, REPLY_MAX_SIZE);
    
    nextChunkToRequest += CHUNK_WINDOW_SIZE;
    
    char* cid_list = new char[REPLY_MAX_SIZE];
    strncpy(cid_list, reply, REPLY_MAX_SIZE);
    
    string entireReply(reply);
    stringstream ss(entireReply);
    string cid;
    cout << "HERE WE GO" << endl;
    while(ss >> cid){
        cout << "Pushing back " << cid.substr(4) << endl;
        cidList.push_back(cid.substr(4));
    }

    return cidList;
}



int ChunkFetcher::readChunkData(vector<string> listOfChunkCIDs){
    ChunkStatus chunkStatuses[CHUNK_WINDOW_SIZE];

    // Number of chunks in the CID List that we assemble
    int numChunks = 0;

    // build the list of chunk CID chunkStatuses (including Dags) to retrieve
    for(vector<string>::iterator it = listOfChunkCIDs.begin(); it != listOfChunkCIDs.end(); ++it) {
        
        // Create DAG for CID
        char* dag = (char *)malloc(MAX_LENGTH_OF_CID_DAG);
        
        // Create a DAG consisting of all known routes to the CID
        // Start with the primary route
        Node n_src;
        Node n_ad(Node::XID_TYPE_AD, videoInformation.getServerLocation(0).getAd().c_str());
        Node n_hid(Node::XID_TYPE_HID, videoInformation.getServerLocation(0).getHid().c_str());
        Node n_cid(Node::XID_TYPE_CID, *it);
        Graph g = n_cid;
        Graph g2 = n_hid * n_cid;
        Graph g3 = n_src * n_ad * n_hid * n_cid;
        g = g + g2;
        g = g + g3;
        
        // Add fall back paths
        for(int i = 1; i < videoInformation.getNumServerLocations(); i++){
            Node n_ad_backup(Node::XID_TYPE_AD, videoInformation.getServerLocation(i).getAd().c_str());
            Node n_hid_backup(Node::XID_TYPE_HID, videoInformation.getServerLocation(i).getHid().c_str());
            Graph fallbackSubGraph = n_hid_backup * n_cid;
            g = g + fallbackSubGraph;
            Graph fallbackGraph = n_src * n_ad_backup * n_hid_backup * n_cid;
            g = g + fallbackGraph;
        }
               
        strcpy(dag, g.dag_string().c_str());
        //string dag2 = "DAG 0 2 -\n"
        //             "AD:1000000000000000000000000000000000000001 1 2 -\n"
        //             "HID:0000000000000000000000000000000000000009 4-\n" 
        //             "AD:1000000000000000000000000000000000000002 3 -\n" 
        //             "HID:0000000000000000000000000000000000000002 4 -\n" ;
        string cidPrefix("CID:");
        string cidThing(*it);         
        //dag2 = dag2 + cidPrefix + cidThing;
        
        //cout << dag2.length() << endl;
        //strcpy(dag, dag2.c_str());  
        cout << dag << endl;
        
        chunkStatuses[numChunks].cidLen = g.dag_string().length();//g.dag_string().length();
        chunkStatuses[numChunks].cid = dag;
        numChunks++;
    }

    say("REQUEST CHUNKS", LVL_DEBUG);
    // BRING LIST OF CHUNKS LOCAL
    if (XrequestChunks(chunkSock, chunkStatuses, numChunks) < 0) {
        cerr << "unable to request chunks" << endl;
        return -1;
    }

    say("LOAD CHUNKS IN", LVL_DEBUG);
    // IDLE AROUND UNTIL ALL CHUNKS ARE READY
    int waitingForChunkCounter = 0;
    while (1) {
        int status = XgetChunkStatuses(chunkSock, chunkStatuses, numChunks);
        //printChunkStatuses(chunkStatuses, numChunks);

        if (status == READY_TO_READ){
            break;
        }
        else if (status < 0) { // REQUEST_FAILED Or INVALID_HASH
            say("error getting chunk status");
            return -1;

        } else if (status == WAITING_FOR_CHUNK) {
            // one or more chunks aren't ready.
            say("waiting... one or more chunks aren't ready yet", LVL_DEBUG);
            //printChunkStatuses(chunkStatuses, numChunks);
            waitingForChunkCounter++;
            
            if (waitingForChunkCounter > NUM_WAITING_MESSAGES_THRESHOLD){
                say("\n*****SWITCHING PRIMARY CID LOCATION*****\n");
                videoInformation.rotateServerLocations();
                
                // this may lead to infinite recursion, if no server location works
                return readChunkData(listOfChunkCIDs);
            }
            
        }
        sleep(1);
    }
    
    say("READ CHUNKS", LVL_DEBUG);

    // READ EACH CHUNK
    for (int i = 0; i < numChunks; i++) {
        char *cid = strrchr(chunkStatuses[i].cid, ':');
        cid++;

        char chunkData[XIA_MAXCHUNK];
        memset(chunkData, 0, XIA_MAXCHUNK);

        // Receive the chunk, and write into data buffer
        int len = 0;
        if ((len = XreadChunk(chunkSock, chunkData, XIA_MAXCHUNK, 0, chunkStatuses[i].cid, chunkStatuses[i].cidLen)) < 0) {
            cerr << "error getting chunk\n";
            return -1;
        }

        chunkQueue->push(new Chunk(chunkData, len));

        free(chunkStatuses[i].cid);
        chunkStatuses[i].cid = NULL;
        chunkStatuses[i].cidLen = 0;
    }
    
    say("DONE READING", LVL_DEBUG);

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

    //Append null character
    reply[n] = 0;

    // Return number of bytes successfully received
    return n;
}


/*
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

void printChunkStatuses(ChunkStatus* chunkStatuses, int numChunks){
    ChunkStatus* curChunkStatus = chunkStatuses;
    for(int i = 0; i < numChunks; i++){
        cout << curChunkStatus->cid << ": " << curChunkStatus->status << endl;
        curChunkStatus++;
    }
}

