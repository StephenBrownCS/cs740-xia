#ifndef __X_CHUNK_SOCKET_STREAM_H_
#define __X_CHUNK_SOCKET_STREAM_H_

#include<iostream>
#include<list>
#include<utility>
#include "Chunk.h"

class XChunkSocketStream : public std::istream{
private:
	// Descriptor to the chunk socket to read from
	int xSocket;
	int chunkSock;
	int bytesReadByLastOperation;
	
	int numChunksInFile;
	int nextChunkToRequest;
	
        // Flag that is checked by good() and is set by retrieveCIDs
        bool reachedEndOfFile;

	const char* const SERVER_AD;
	const char* const SERVER_HID;
	
	// Represents the current chunk being copied from
	Chunk* currentChunk;
	
	// Represents the number of chunks that have already 
	// been read from the current chunk
	int numBytesReadFromCurrentChunk;
	
	// Container which represents the next chunk
	std::list< Chunk* >chunkQueue;
	
public:
	
	XChunkSocketStream(int xSocket, int numChunksInFile, const char* serverAd, const char* serverHid);
	
	/** Overridden method of istream
	 * Returns the number of bytes read by the last operation
	 *
	*/
	int gcount();

	//Overridden
	bool good();
	
	/** Overridden method of istream
	 * Reads up to numBytesRequested bytes into the buffer
	*/
	std::istream& read(char* buffer, std::streamsize numBytesRequested);
	
private:

    /**
     * Requests for a list of CIDs from the server and then fetches the chunks 
     * and places them in the chunkQueue.
    */
    void fetchChunks();


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
	int numBytesReady();
};

#endif
