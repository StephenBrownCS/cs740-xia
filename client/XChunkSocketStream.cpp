#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <pthread.h>

#include "Xsocket.h"
#include "dagaddr.hpp"
#include "XChunkSocketStream.h"
#include "ChunkFetcher.h"

using namespace std;


const int CHUNK_WINDOW_SIZE = 10;

//TODO: WHY?
const int REPLY_MAX_SIZE = 512;



XChunkSocketStream::XChunkSocketStream(int xSocket, int numChunksInFile, const char* serverAd, const char* serverHid):
    SERVER_AD(serverAd),
    SERVER_HID(serverHid)
{

    currentChunk = NULL;
    bytesReadByLastOperation = 0;
    numBytesReadFromCurrentChunk = 0;
    reachedEndOfFile = false;
    
    // CREATE CHUNK FETCHER WORKER THREAD
    chunkFetcher = new ChunkFetcher(xSocket, numChunksInFile, serverAd, serverHid);
    pthread_t chunkFetcherThread;
    int ret = pthread_create(&chunkFetcherThread, NULL, 
                            ChunkFetcher::fetchChunks, (void *) chunkFetcher);
    if (ret < 0){
        cerr << "Could not create the chunk fetcher worker thread" << endl;
        exit(-1);
    }
}
å


int XChunkSocketStream::gcount(){
    return bytesReadByLastOperation;
}


bool XChunkSocketStream::good(){
    return !reachedEndOfFile;
}



istream& XChunkSocketStream::read(char* buffer, streamsize numBytesRequested){
    bytesReadByLastOperation = 0;
   
    if (currentChunk == NULL){
        currentChunk = chunkFetcher->getNextChunkFromQueue();
        if(currentChunk == NULL){
            cout << "Stream recognizes that it is the end of the file" << endl;
            //We must have reached the end of the file
            reachedEndOfFile = true;
            return *this;
        }
        
        numBytesReadFromCurrentChunk = 0;
    }

    // Continue until we get all the bytes we want
    while(bytesReadByLastOperation < numBytesRequested){

        // If current chunk is all used up, move to next chunk
        if(numBytesReadFromCurrentChunk >= currentChunk->size()){
            // Get the next chunk
            delete currentChunk;
            currentChunk = chunkFetcher->getNextChunkFromQueue();
            if(currentChunk == NULL){
                cout << "Stream recognizes that it is the end of the file" << endl;
                //We must have reached the end of the file
                reachedEndOfFile = true;
                return *this;
            }
        
            numBytesReadFromCurrentChunk = 0;
        }

        // copy bytes from the current chunk into buffer
        while(numBytesReadFromCurrentChunk < currentChunk->size() && bytesReadByLastOperation < numBytesRequested){
            *buffer++ = (*currentChunk)[numBytesReadFromCurrentChunk++];
            bytesReadByLastOperation++;
        }
    }
    return *this;
}

