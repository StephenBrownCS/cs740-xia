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
#include "ClientConfig.h"

using namespace std;

#define VERSION "v1.0"
#define TITLE "XIA Chunk File Client"
#define SERVER_NAME "www_s.video.com.xia"

// global configuration options
int VERBOSE = 1;
char *SERVER_AD;
char *SERVER_HID;



/*
** Receive number of chunks
*/
int receiveNumberOfChunks(int sock);


/**
 * Useful for debugging: prints the CID and status of each chunkStatus
 *
*/
void printChunkStatuses(ChunkStatus* chunkStatuses, int numChunks);




// ***************************************************************************
// ******                    MAIN METHOD                                 *****
// ***************************************************************************

int main(){
    int sock;
	string videoName = "BigBuckBunny"; //Hard-coded

    say ("\n%s (%s): started\n", TITLE, VERSION);

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
	char* p = strchr(SERVER_AD, ' ');
    *p = 0;
    SERVER_HID = p + 1;
    SERVER_HID = strstr(SERVER_HID, "HID:");
    p = strchr(SERVER_HID, ' ');
    *p = 0;

    // send the request for the number of chunks
    cout << "Sending request for number of chunks" << endl;
    string numChunksReqStr = "get numchunks " + videoName;
    sendCmd(sock, numChunksReqStr.c_str());
    
    // GET NUMBER OF CHUNKS
    // Receive the reply string
    int numChunksInFile = receiveNumberOfChunks(sock);
    cout << "Received number of chunks: " << numChunksInFile << endl;

    // STREAM THE VIDEO
    XChunkSocketStream chunkSocketStream(sock, numChunksInFile, SERVER_AD, SERVER_HID);
    PloggOggDecoder oggDecoder;
    oggDecoder.play(chunkSocketStream);   

    say("shutting down\n");
    sendCmd(sock, "done");
    Xclose(sock);
    return 0;
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

