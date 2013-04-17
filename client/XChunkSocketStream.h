#ifndef __X_CHUNK_SOCKET_STREAM_H_
#define __X_CHUNK_SOCKET_STREAM_H_

#include<iostream>
#include<queue>

using namespace std;

class XChunkSocketStream : public istream{
private:
	// Descriptor to the chunk socket to read from
	int xSocket;
	int chunkSock;
	int bytesReadByLastOperation;
	
	int numChunksInFile;
	int nextChunkToRequest;
	
	const char* const SERVER_AD;
	const char* const SERVER_HID;
	
	// Represents the current chunk being copied from
	char* currentChunk;
	
	// Represents the number of chunks that have already 
	// been read from the current chunk
	int numBytesReadFromCurrentChunk;
	
	// Container which represents the next chunks
	queue<char* > chunkQueue;
	
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
	istream& read(char* buffer, streamsize numBytesRequested);
	
private:
	/**
	 * Asks the server for a list of CIDs
	 * Receives the reply and returns the list
	*/
	char* retrieveCIDs();
	
	/**
	 * Fetches the chunk data referred to by the list of CIDs
	 * Returns the number of chunks read in
	*/
	int readChunkData(char* listOfChunkCIDs);
	
	
	int sendCmd(const char *cmd);

	int receiveReply(char *reply, int size);
	
};

#endif
