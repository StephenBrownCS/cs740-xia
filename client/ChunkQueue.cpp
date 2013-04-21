
#include <iostream>
#include <pthread.h>
#include <cassert>
#include "ChunkQueue.h"

using namespace std;

ChunkQueue::ChunkQueue(){
    int ret = pthread_mutex_init(queueLock, NULL);
    if (ret < 0){
        cerr << "Could not create a mutex for the chunk Queue" << endl;
        exit(-1);
    }
}


ChunkQueue::~ChunkQueue(){
    pthread_mutex_destroy(queueLock);
}


int ChunkQueue::size(){
    int ret = pthread_mutex_lock(queueLock);
    if( ret < 0){
        cerr << "Could not lock queue lock!" << endl;
    }
    
    int size = chunkQueue.size();
    
    ret = pthread_mutex_unlock(queueLock);
    if( ret < 0){
        cerr << "Could not unlock queue lock!" << endl;
    }
    
    return size;
}

void ChunkQueue::pop(){
    int ret = pthread_mutex_lock(queueLock);
    if( ret < 0){
        cerr << "Could not lock queue lock!" << endl;
    }
    
    assert(chunkQueue.size() != 0);
    chunkQueue.pop_front();
    
    ret = pthread_mutex_unlock(queueLock);
    if( ret < 0){
        cerr << "Could not unlock queue lock!" << endl;
    }
}

Chunk* ChunkQueue::front(){
    int ret = pthread_mutex_lock(queueLock);
    if( ret < 0){
        cerr << "Could not lock queue lock!" << endl;
    }
    
    assert(chunkQueue.size() != 0);
    Chunk* firstChunk = chunkQueue.front();
    
    ret = pthread_mutex_unlock(queueLock);
    if( ret < 0){
        cerr << "Could not unlock queue lock!" << endl;
    }
    
    return firstChunk;
}

void ChunkQueue::push(Chunk* chunk){
    int ret = pthread_mutex_lock(queueLock);
    if( ret < 0){
        cerr << "Could not lock queue lock!" << endl;
    }
    
    chunkQueue.push_back(chunk);
    
    ret = pthread_mutex_unlock(queueLock);
    if( ret < 0){
        cerr << "Could not unlock queue lock!" << endl;
    }
}
