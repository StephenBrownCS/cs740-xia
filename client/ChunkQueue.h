#ifndef CHUNK_QUEUE_H_
#define CHUNK_QUEUE_H_

#include <list>
#include <pthread.h>
#include "Chunk.h"


/**
 * A thread-safe wrapper on an stl list for use as our Chunk Queue of Chunk*
 *
*/
class ChunkQueue{
	std::list< Chunk* >chunkQueue;

    pthread_mutex_t* queueLock;

public:
    ChunkQueue();
    
    ~ChunkQueue();

    int size();
    
    void pop();
    
    Chunk* front();
    
    void push(Chunk* chunk);
    
private:
    // No copy or assignment
    ChunkQueue(ChunkQueue& other);
	ChunkQueue & operator=(ChunkQueue & other);
};

#endif
