/*
    Represents a Stream of data coming in from an X Socket. Creates and uses 
    a chunk socket to receive the actual content.

    This wrapper class was necessary since the PloggOggDecoder class works with 
    istreams.
*/


#ifndef __X_CHUNK_SOCKET_STREAM_H_
#define __X_CHUNK_SOCKET_STREAM_H_

#include <iostream>
#include <list>
#include <utility>
#include "Chunk.h"

// Forward Reference
class ChunkFetcher;

class XChunkSocketStream : public std::istream{
private:
	int bytesReadByLastOperation;

    // Flag that is checked by good() and is set by retrieveCIDs
    bool reachedEndOfFile;

	const char* const SERVER_AD;
	const char* const SERVER_HID;
	
	ChunkFetcher* chunkFetcher;
	
	// Represents the current chunk being copied from
	Chunk* currentChunk;
	
	// Represents the number of chunks that have already 
	// been read from the current chunk
	int numBytesReadFromCurrentChunk;
	
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
};

#endif
