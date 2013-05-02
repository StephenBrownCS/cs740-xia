
/*
	Video Client
	This Video Client will stream video from the video server

	Authors: Ben Bramble, Stephen Brown, and Big Buck Bunny
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "Xsocket.h"
#include "dagaddr.hpp"
#include "Utility.h"
#include "XChunkSocketStream.h"
#include "PloggOggDecoder.h"
#include "ClientConfig.h"
#include "VideoInformation.h"

using namespace std;

const string SERVER_NAME = "www_s.video.com.xia";


/*
** Receive number of chunks
*/
VideoInformation receiveVideoInformation(int sock);




// ***************************************************************************
// ******                    MAIN METHOD                                 *****
// ***************************************************************************

int main(){
    printHostInformation();
    
    if(argc > 1){
        if(strcmp(argv[1], "-v") == 0)
            VERBOSE = true;
    }
    
    int sock;
	string videoName = "BigBuckBunny"; //Hard-coded

    // Get the DAG for the Server
    sockaddr_x server_dag;
    socklen_t dag_length = sizeof(server_dag);
    if (XgetDAGbyName(SERVER_NAME.c_str(), &server_dag, &dag_length) < 0){
        die(-1, "unable to locate: %s\n", SERVER_NAME.c_str());
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

	//Format is <numChunks> <AD:XXXX> <HID:YYYY> ...
	// Parse it!
	string buffer_str(buffer);
	stringstream ss(buffer_str);
	string numChunks;
	ss >> numChunks;
	say("numChunks: " + numChunks);
	
	VideoInformation videoInformation(atoi(numChunks.c_str()));
	
	// There may be multiple ad-hid's listed (multiple locations for 
	// content servers)
	say("Content Server Locations:");
	string ad, hid;
	while (ss >> ad >> hid){
		say("\tAD: " + ad + " " + "HID: " + hid);
		ServerLocation location(ad + " " + hid);
		videoInformation.addServerLocation(location);
	} 

    delete[] buffer;
    return videoInformation;
}



