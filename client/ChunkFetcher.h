/*
    Instances of this class will go out and fetch data. A ChunkFetcher will 
    use the xSocket passed in at construction time to request the CID lists. 
    It will also create its own Chunk Socket to fetch the actual data.

    This class is intended to be used in a multi-threaded context, where one 
    thread fetches the chunks, and another thread "calls in" using 
    getNextChunkFromQueue().
*/

#ifndef CHUNK_FETCHER_H_
#define CHUNK_FETCHER_H_

#include <iostream>
#include <utility>
#include <vector>
#include "VideoInformation.h"

class Chunk;
class ChunkQueue;

class ChunkFetcher{
    // Descriptor to the socket we use to get our CID lists from
	int xSocket;
	
	VideoInformation videoInformation;
	
	// Descriptor to our chunk socket, from which we get the actual data
	int chunkSock;
	
	int nextChunkToRequest;
	
	// This flag is set to true when there are no more CIDs left to request
    volatile bool reachedEndOfFile;
	
	// Thread-safe Container which represents the next chunk
	ChunkQueue* chunkQueue;
    
public:
    ChunkFetcher(int xSocket, VideoInformation & videoInformation);
    
    /*
     * Destructor
     * Closes the chunk socket and deletes the chunkQueue
     * Does not close the xSocket
    */
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
	std::vector<std::string> retrieveCIDs();
	
	/**
	 * Fetches the chunk data referred to by the list of CIDs
	 * and places the chunks in the chunkQueue
	 * Returns the number of chunks read in
	*/
	int readChunkData(std::vector<std::string> listOfChunkCIDs);
	
	
	/**
	 * Creates a Dag consisting of a route from src to cid, src to primary hid 
	 * to cid, src to primary ad to primary hid to cid, and paths from src to 
	 * each ad to the subsequent ad to cid, and paths from src to hid to the 
	 * subsequent ad to cid.
	*/
	std::string createDag(const std::string & cid);
	
	
	/**
	 * Sends command string to the CID directory server that we are communicating
	 * with.
	*/
	int sendCmd(const char *cmd);

    /**
     * Receives reply from the server, and places it in the reply buffer.
    */
	int receiveReply(char *reply, int size);
	
	// Returns the number of bytes ready in the queue + bytes remaining in 
	// current chunk, if applicable
	// int numBytesReady();
	
	// No copy or assignment
	ChunkFetcher(ChunkFetcher& other);
	ChunkFetcher & operator=(ChunkFetcher & other);
};

#endif
