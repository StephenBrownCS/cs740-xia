/* ts=4 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "Xsocket.h"
#include "dagaddr.hpp"
#include "Utility.h"
#include "XChunkSocketStream.h"
#include "PloggOggDecoder.h"

using namespace std;

#define VERSION "v1.0"
#define TITLE "XIA Chunk File Client"
#define SERVER_NAME "www_s.video.com.xia"

// TODO: THIS IS NOT THE ONLY PLACE WHERE THESE CONSTANTS ARE MADE
// NEED TO CREATE SEPARATE CONSTANTS OR CONFIG FILE

//TODO: WHY IS THIS ALWAYS 10??????
const int CHUNK_WINDOW_SIZE = 1;

//TODO: WHY?
const int REPLY_MAX_SIZE = 512;


// global configuration options
int VERBOSE = 1;
char *SERVER_AD;
char *SERVER_HID;





int sendCmd(int sock, const char *cmd);

/**
 * Receive reply from the server
 *
*/
int receiveReply(int sock, char *reply, int size);

/*
** Receive number of chunks
*/
int receiveNumberOfChunks(int sock);

/**
 * Build a ChunkStatus list
 * Request the chunks and bring them local
 * Read the chunks into the program and write them out to a file
*/
int getChunkData(int chunkSock, char *chunks);

/**
 * Useful for debugging: prints the CID and status of each chunkStatus
 *
*/
void printChunkStatuses(ChunkStatus* chunkStatuses, int numChunks);




// ***************************************************************************
// ******                    MAIN METHOD                                 *****
// ***************************************************************************

int main(){
    int sock, chunkSock;
    int offset;
    char *p;
    const char *srcFile;
    const char *destFile;
    char cmd[512];
    int status = 0;

    say ("\n%s (%s): started\n", TITLE, VERSION);

    // Hard-coded for now
    srcFile = "inputFile";
    destFile = "destFile";

    // Get the DAG for the Server
    
    sockaddr_x server_dag;
    socklen_t dag_length = sizeof(server_dag);
    if (XgetDAGbyName(SERVER_NAME, &server_dag, &dag_length) < 0){
        die(-1, "unable to locate: %s\n", SERVER_NAME);
    }

    // create a STREAM socket
    // XSOCK_STREAM is for reliable communications (SID)
    if ((sock = Xsocket(AF_XIA, XSOCK_STREAM, 0)) < 0)
         die(-1, "Unable to create the listening socket\n");

    // Connect the socket to the server dag
    if (Xconnect(sock, (struct sockaddr*)&server_dag, dag_length) < 0) {
        Xclose(sock);
         die(-1, "Unable to bind to the dag: %s\n", server_dag);
    }

	// save the AD and HID for later. This seems hacky
	// we need to find a better way to deal with this
	Graph g(&server_dag);
	char sdag[1024];
	strncpy(sdag, g.dag_string().c_str(), sizeof(sdag));
	SERVER_AD = strstr(sdag, "AD:");
	p = strchr(SERVER_AD, ' ');
	*p = 0;
	SERVER_HID = p + 1;
	SERVER_HID = strstr(SERVER_HID, "HID:");
	p = strchr(SERVER_HID, ' ');
	*p = 0;

    // send the request for the number of chunks
	cout << "Sending request for number of chunks" << endl;
    sprintf(cmd, "get %s",  "numchunks");
    sendCmd(sock, cmd);
	

    // GET NUMBER OF CHUNKS
    // Receive the reply string
	int numChunksInFile = receiveNumberOfChunks(sock);
	cout << "Received number of chunks: " << numChunksInFile << endl;

    // Create chunk socket
    // We will use this to receive chunks
    if ((chunkSock = Xsocket(AF_XIA, XSOCK_CHUNK, 0)) < 0)
        die(-1, "unable to create chunk socket\n");

    // Continue until we have received the entire file
	cout << "Begin Chunk Transfer!" << endl;
    offset = 0;
    while (offset < numChunksInFile) {
		// GET LIST OF CIDs FROM SERVER
		// Determine how many chunks to ask for
        int numToReceive = CHUNK_WINDOW_SIZE;
        if (numChunksInFile - offset < numToReceive)
            numToReceive = numChunksInFile - offset;

        // tell the server we want a list of <numToReceive> cids starting at location <offset>
        sprintf(cmd, "block %d:%d", offset, numToReceive);
		cout << "Sending Chunk Request: " << cmd << endl;
        sendCmd(sock, cmd);

		// Server replies with "OK: " followed by a list of CIDs
        char reply[REPLY_MAX_SIZE];
        receiveReply(sock, reply, sizeof(reply));
		cout << "Received Reply: " << reply << endl;
        
        offset += CHUNK_WINDOW_SIZE;

		// GET THE ACTUAL CHUNK DATA FOR THE LIST OF CHUNK CIDs WE RECEIVED
		// Reply starts at the 4th byte, since it starts with "OK: "
		// TODO: Check that we didn't get rid of the "OK: " **********************************
        if (getChunkData(chunkSock, &reply[4]) < 0) {
            status= -1;
            break;
        }
    }

    if (status < 0) {
        unlink(srcFile);
    }

    say("shutting down\n");
    sendCmd(sock, "done");
    Xclose(sock);
    Xclose(chunkSock);
    return status;
}


int getChunkData(int chunkSock, char* listOfChunkCIDs)
{
    ChunkStatus chunkStatuses[CHUNK_WINDOW_SIZE];
    char *chunk_ptr = listOfChunkCIDs;
    
    // Number of chunks in the CID List that we assemble
    int numChunks = 0;
    
	cout << "Getting File Data" << endl;
    // build the list of chunk CID chunkStatuses (including Dags) to retrieve
    char* next = NULL;
    while ((next = strchr(chunk_ptr, ' '))) {
        *next = 0;

		// Create a dag by using the CID pointed to by chunk_ptr
        char* dag = (char *)malloc(512);
        sprintf(dag, "RE ( %s %s ) CID:%s", SERVER_AD, SERVER_HID, chunk_ptr);
        //printf("getting %s\n", chunk_ptr);
        chunkStatuses[numChunks].cidLen = strlen(dag);
        chunkStatuses[numChunks].cid = dag;
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


    // BRING LIST OF CHUNKS LOCAL
    say("requesting list of %d chunks\n",numChunks);
    if (XrequestChunks(chunkSock, chunkStatuses, numChunks) < 0) {
        say("unable to request chunks\n");
        return -1;
    }


    // IDLE AROUND UNTIL ALL CHUNKS ARE READY
    say("checking chunk status\n");
    while (1) {
        int status = XgetChunkStatuses(chunkSock, chunkStatuses, numChunks);
		printChunkStatuses(chunkStatuses, numChunks);

        if (status == READY_TO_READ){
            break;
        }
        else if (status < 0) { // REQUEST_FAILED Or INVALID_HASH
            say("error getting chunk status\n");
            return -1;

        } else if (status == WAITING_FOR_CHUNK) {
            // one or more chunks aren't ready.
            say("waiting... one or more chunks aren't ready yet\n");
        }
        sleep(1);
    }

    say("all chunks ready\n");


    // RECEIVE EACH CHUNK
    for (int i = 0; i < numChunks; i++) {
        char *cid = strrchr(chunkStatuses[i].cid, ':');
        cid++;
        say("reading chunk %s\n", cid);
        
        // Receive the chunk, and write into data buffer
        char data[XIA_MAXCHUNK];
        int len = 0;
        if ((len = XreadChunk(chunkSock, data, sizeof(data), 0, chunkStatuses[i].cid, chunkStatuses[i].cidLen)) < 0) {
            say("error getting chunk\n");
            return -1;
        }

        free(chunkStatuses[i].cid);
        chunkStatuses[i].cid = NULL;
        chunkStatuses[i].cidLen = 0;
    }

    return numChunks;
}









int sendCmd(int sock, const char *cmd)
{
    int n;

    if ((n = Xsend(sock, cmd,  strlen(cmd), 0)) < 0) {
        Xclose(sock);
         die(-1, "Unable to communicate with the server\n");
    }

    return n;
}

int receiveReply(int sock, char *reply, int size)
{
    int n;

    // Receive (up to) size bytes from the socket, write to reply
    if ((n = Xrecv(sock, reply, size, 0))  < 0) {
        Xclose(sock);
        die(-1, "Unable to communicate with the server\n");
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


int receiveNumberOfChunks(int sock)
{
	char* buffer = new char[REPLY_MAX_SIZE];
	
    // Receive (up to) size bytes from the socket, write to reply
    if (Xrecv(sock, buffer, sizeof(buffer), 0)  < 0) {
        Xclose(sock);
        die(-1, "Unable to communicate with the server\n");
    }

	int numberOfChunks = atoi(buffer);
	delete[] buffer;
	return numberOfChunks;
}


void printChunkStatuses(ChunkStatus* chunkStatuses, int numChunks){
	ChunkStatus* curChunkStatus = chunkStatuses;
	for(int i = 0; i < numChunks; i++){
		cout << curChunkStatus->cid << ": " << curChunkStatus->status << endl;
		curChunkStatus++;
	}
}




