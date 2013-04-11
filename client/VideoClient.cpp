/* ts=4 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "Xsocket.h"
#include "dagaddr.hpp"

using namespace std;

#define VERSION "v1.0"
#define TITLE "XIA Chunk File Client"
#define SERVER_NAME "www_s.video.com.xia"

//TODO: WHY IS THIS ALWAYS 10??????
const int CHUNK_WINDOW_SIZE = 10;

//TODO: WHY?
const int REPLY_MAX_SIZE = 512;


// global configuration options
int VERBOSE = 1;
char *SERVER_AD;
char *SERVER_HID;



/*
** write the message to stdout unless in quiet mode
*/
void say(const char *fmt, ...);

/*
** write the message to stdout, and exit the app
*/
void die(int ecode, const char *fmt, ...);

int sendCmd(int sock, const char *cmd);

int receiveReply(int sock, char *reply, int sz);

/*
** Receive number of chunks
*/
int receiveNumberOfChunks(int sock);

int getFileData(int chunkSock, FILE *fd, char *chunks);



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
    
    // I have no idea what sock_addr is used for... it was added in the API update
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

    // OLD WAY
    /*
    SERVER_AD = strchr(dag, ' ') + 1;
    p = strchr(SERVER_AD, ' ');
    *p = 0;
    SERVER_HID = p + 1;
    p = strchr(SERVER_HID, ' ');
    *p = 0;
    */

	// NEW WAY
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

    // Open a file for writing
    // TODO: Get rid of this
    FILE *file = fopen(destFile, "w");

    // RECEIVE EACH CHUNK
	cout << "Begin Chunk Transfer!" << endl;
    offset = 0;
    while (offset < numChunksInFile) {
        int numToReceive = CHUNK_WINDOW_SIZE;
        if (numChunksInFile - offset < numToReceive)
            numToReceive = numChunksInFile - offset;

        // tell the server we want a list of <numToReceive> cids starting at location <offset>
        sprintf(cmd, "block %d:%d", offset, numToReceive);
		cout << "Sending Chunk Request: " << cmd << endl;
        sendCmd(sock, cmd);

        char reply[REPLY_MAX_SIZE];
        receiveReply(sock, reply, sizeof(reply));
		cout << "Received Reply: " << reply << endl;
        
        offset += CHUNK_WINDOW_SIZE;

        // TODO: Instead of getFileData, we will want to do something 
        // with each video chunk
        if (getFileData(chunkSock, file, &reply[4]) < 0) {
            status= -1;
            break;
        }
    }
    
    // TODO: Get rid of this
    fclose(file);

    if (status < 0) {
        unlink(srcFile);
    }

    say("shutting down\n");
    sendCmd(sock, "done");
    Xclose(sock);
    Xclose(chunkSock);
    return status;
}


int getFileData(int chunkSock, FILE *fd, char *chunks)
{
    ChunkStatus chunkStatuses[CHUNK_WINDOW_SIZE];
    char *chunk_ptr = chunks;
    
    // Number of chunks in the CID List that we assemble
    int numChunks = 0;
    

    // build the list of chunk CID chunkStatuses (including Dags) to retrieve
    char* next = NULL;
    while ((next = strchr(chunk_ptr, ' '))) {
        *next = 0;

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
    {
        char* dag = (char *) malloc(512);
        sprintf(dag, "RE ( %s %s ) CID:%s", SERVER_AD, SERVER_HID, chunk_ptr);
        //printf("getting %s\n", chunk_ptr);
        chunkStatuses[numChunks].cidLen = strlen(dag);
        chunkStatuses[numChunks].cid = dag;
        numChunks++;
    }


    // BRING LIST OF CHUNKS LOCAL
    say("requesting list of %d chunks\n",numChunks);
    if (XrequestChunks(chunkSock, chunkStatuses, numChunks) < 0) {
        say("unable to request chunks\n");
        return -1;
    }


    // IDLE AROUND UNTIL ALL CHUNKS ARE READY
    say("checking chunk status\n");
    while (1) {
        int status = XgetChunkStatuses(chunkSock, chunkStatuses,numChunks);

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

        // write the chunåk to disk
        //say("writing %d bytes of chunk %s to disk\n", len, cid);
        fwrite(data, 1, len, fd);

        free(chunkStatuses[i].cid);
        chunkStatuses[i].cid = NULL;
        chunkStatuses[i].cidLen = 0;
    }

    return numChunks;
}







void say(const char *fmt, ...)
{
    if (VERBOSE) {
        va_list args;

        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

void die(int ecode, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "%s: exiting\n", TITLE);
    exit(ecode);
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
    if (strncmp(reply, "OK:", 3) != 0) {
        die(-1, "%s\n", reply);
    }

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





