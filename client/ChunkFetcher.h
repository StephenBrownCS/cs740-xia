#ifndef CHUNK_FETCHER_H_
#define CHUNK_FETCHER_H_

#include <iostream>
#include <utility>

class Chunk;
class ChunkQueue;

class ChunkFetcher{
    // Descriptor to the socket to read from
	int xSocket;
	int chunkSock;
	
	int numChunksInFile;
	int nextChunkToRequest;
	
    volatile bool reachedEndOfFile;

	const char* const SERVER_AD;
	const char* const SERVER_HID;
	
	// Container which represents the next chunk
	ChunkQueue* chunkQueue;
    
    
public:
    ChunkFetcher(int xSocket, int numChunksInFile, const char* serverAd, const char* serverHid);
    ~ChunkFetcher();

    /**
     * Removes the next chunk from the Queue and returns it
     * The Chunk returned is dynamically allocated and must be deleted
     * when the user is done with it
     * Returns NULL if the end of the file has been reached
    */
    Chunk* getNextChunkFromQueue();

    /**
     * Begins retrieving chunks window-by-window, and maintains the chunk queue 
     * at a certain maximum size. (TODO: how is that specified?), and ensures 
     * that there are always chunks available
     *
     * In order to work with pthreads, must be a static void* (void *) function
    */
    static void* fetchChunks(void* chunkFetcher);

private:

    /**
     * Requests for a list of CIDs from the server and then fetches the chunks 
     * and places them in the chunkQueue.
    */
    void fetchChunkWindow();
    
    
	/**
	 * Asks the server for a list of CIDs
	 * Server return format: CID:<CID 1> CID:<CID2> etc.
	 * Receives the reply and returns a dynamically-allocated char* buffer that 
	 * represents the list of CIDs
         * Returns NULL if we have already reached the end of the file
	*/
	char* retrieveCIDs();
	
	/**
	 * Fetches the chunk data referred to by the list of CIDs
	 * and places the chunks in the chunkQueue
	 * Returns the number of chunks read in
	*/
	int readChunkData(char* listOfChunkCIDs);
	
	int sendCmd(const char *cmd);

	int receiveReply(char *reply, int size);
	
	// Returns the number of bytes ready in the queue + bytes remaining in 
	// current chunk, if applicable
	// int numBytesReady();
	
	// No copy or assignment
	ChunkFetcher(ChunkFetcher& other);
	ChunkFetcher & operator=(ChunkFetcher & other);
};

#endif
