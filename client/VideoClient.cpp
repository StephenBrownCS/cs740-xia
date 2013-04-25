/* ts=4 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <sstream>
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


struct VideoInformation{
    int numChunks;
    
    // a list of AD-HIDs
    vector<string> hosts;
};



/*
** Receive number of chunks
*/
VideoInformation receiveVideoInformation(int sock);

/**
 * Useful for debugging: prints the CID and status of each chunkStatus
 *
*/
void printChunkStatuses(ChunkStatus* chunkStatuses, int numChunks);

string extractDagAd(sockaddr_x dagStr);

string extractDagHid(sockaddr_x dagStr);


// ***************************************************************************
// ******                    MAIN METHOD                                 *****
// ***************************************************************************

int main(){
    printHostInformation();
    
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

    string serverAd = extractDagAd(server_dag);
    string serverHid = extractDagHid(server_dag);

    // send the request for the number of chunks
    cout << "Sending request for number of chunks" << endl;
    string numChunksReqStr = "get numchunks " + videoName;
    sendCmd(sock, numChunksReqStr.c_str());
    
    // GET NUMBER OF CHUNKS
    VideoInformation videoInformation = receiveVideoInformation(sock);
    cout << "Received number of chunks: " << videoInformation.numChunks << endl;

    // STREAM THE VIDEO
    XChunkSocketStream chunkSocketStream(sock, videoInformation);
    PloggOggDecoder oggDecoder;
    oggDecoder.play(chunkSocketStream);   

    say("shutting down\n");
    sendCmd(sock, "done");
    Xclose(sock);
    return 0;
}



VideoInformation receiveVideoInformation(int sock)
{
    char* buffer = new char[REPLY_MAX_SIZE];
    
    // Receive (up to) size bytes from the socket, write to reply
    if (Xrecv(sock, buffer, REPLY_MAX_SIZE, 0)  < 0) {
        Xclose(sock);
        die(-1, "Unable to communicate with the server\n");
    }

    cout << buffer << endl;
	string buffer_str(buffer);
	stringstream ss(buffer_str);
	string numChunks;
	ss >> numChunks;
	cout << numChunks;
	
	VideoInformation videoInformation;
    result.numChunks = atoi(numChunks.c_str());
	
	string ad, hid;
	while (ss >> ad >> hid){
		cout << ad << endl;
		cout << hid << endl;
		result.hosts.push_back(ad + " " + hid);
	} 


    delete[] buffer;
    return videoInformation;
}


void printChunkStatuses(ChunkStatus* chunkStatuses, int numChunks){
    ChunkStatus* curChunkStatus = chunkStatuses;
    for(int i = 0; i < numChunks; i++){
        cout << curChunkStatus->cid << ": " << curChunkStatus->status << endl;
        curChunkStatus++;
    }
}


string extractDagAd(sockaddr_x dagStr){
	Graph g(&dagStr);
	string dag = g.dag_string();
	
	int beginPos = dag.find("AD:");
	int endPos = dag.find(" ", beginPos);
	dag = dag.substr(beginPos, endPos - beginPos);
	
	return dag;
}

string extractDagHid(sockaddr_x dagStr){
	Graph g(&dagStr);
	string dag = g.dag_string();
	
	int beginPos = dag.find("HID:");
	int endPos = dag.find(" ", beginPos);
	dag = dag.substr(beginPos, endPos - beginPos);
	
	return dag;
}



