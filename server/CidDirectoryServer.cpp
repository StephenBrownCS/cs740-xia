
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include "Xsocket.h"
#include "Utility.h"

#define DEBUG

#define VERSION "v1.0"
#define TITLE "XIA Video Server"

#define AD1   "AD:1000000000000000000000000000000000000001"
#define HID1 "HID:0000000000000000000000000000000000000001"
#define SID_VIDEO "SID:1f10000001111111111111111111111110000056"
#define DAG "RE %s %s %s"

#define CHUNKSIZE (1024)
#define SNAME "www_s.video.com.xia"

using namespace std;

//GLOBAL CONFIGURATION OPTIONS
string VIDEO_NAME = "../../xia-core/applications/demo/web_demo/resources/video.ogv";
//string VIDEO_NAME = "../../small.ogv";

map<string, vector<string>* > CIDlist;
vector<string> videoList;

/*
** handle the request from the client and return the requested data
*/
static void *processRequest (void *socketid);


/**
 * Returns true if the string indicates a request for 
 * the number of chunks for a certain video.
*/
bool isNumChunksRequest(string requestStr);

/**
 * Returns true if the string indicates a request for the 
 * the connection to terminate.
*/
bool isTerminationRequest(string requestStr);

/**
 * Returns true if the string indicates a request for the list
 * of videos
*/
bool isVideoSelectionRequest(string requestStr);


/*
** display cmd line options and exit
*/
static void help(const char *name);

/*
** configure the app
*/
static void getConfig(int argc, char** argv);


static void readInCIDLists();


//***************************************************************************
//******                    MAIN METHOD                                 *****
//***************************************************************************

int main(int argc, char *argv[])
{
    int sock;

    getConfig(argc, argv);

    // Initialize our list of videos
    videoList.push_back("BigBuckBunny");

    // Read in the CID lists for each video from disk
    readInCIDLists();

    // create a socket for listening on
    if ((sock = Xsocket(AF_XIA, SOCK_STREAM, 0)) < 0){
        die(-1, "Unable to create the listening socket\n");
    }

    struct addrinfo *addrInfo;
    if (Xgetaddrinfo(NULL, SID_VIDEO, NULL, &addrInfo) < 0)
        die(-1, "Unable to create the local dag\n");

    sockaddr_x* dag = (sockaddr_x*) addrInfo->ai_addr;

    // register this service name to the name server 
    if (XregisterName(SNAME, dag) < 0 )
        perror("name register");    

    // Bind our socket to the dag
    if(Xbind(sock, (struct sockaddr*)dag, sizeof(sockaddr_x)) < 0){
        die(-1, "Unable to bind to the dag: %s\n", dag);
    }

    // we're done with this
    Xfreeaddrinfo(addrInfo);
    dag = NULL;

    while (1) {
        say("\nListening...\n");

        int* acceptSock = new int();
        if ((*acceptSock = Xaccept(sock, NULL, NULL)) < 0)
            die(-1, "accept failed\n");

        say("We have a new connection\n");

        // handle the connection in a new thread
        pthread_t client;
        pthread_create(&client, NULL, processRequest, (void *)acceptSock);
    }

    Xclose(sock); // we should never reach here!

    return 0;
}


void *processRequest (void *socketid)
{
    int n;
    int *sock = (int*)socketid;
    int acceptSock = *sock; 
	string videoName;

    bool clientSignaledToClose = false;

    while(!clientSignaledToClose){
        char SIDReq[1024];
        memset(SIDReq, 0, sizeof(SIDReq));

        //Receive packet
        say("Receiving packet...\n");
        if ((n = Xrecv(acceptSock, SIDReq, sizeof(SIDReq), 0)) <= 0) {
            cerr << "Xrecv failed!" << endl;
            Xclose(acceptSock);
            delete sock;
            return NULL;
        }

        string SIDReqStr(SIDReq);
        cout << "Got request: " << SIDReqStr << endl;
        // if the request is about number of chunks return number of chunks
        // since this is first time, you would return along with header

        // If Request contains "numchunks", return number of CID's.
        if(isNumChunksRequest(SIDReqStr)){
			// Get Video Name out of the request
			string prefix = "get numchunks ";
			videoName = SIDReqStr.substr(prefix.length());
			
			cout << " Request asks for number of chunks: " << videoName << endl;

			//Figure out what video they would like
            			
			// Check to see if this video is the one that the user is looking for
			if(CIDlist.find(videoName) != CIDlist.end()){
				// Print the # of chunks to a String
	            stringstream yy;
	            yy << CIDlist[videoName]->size();
	            string cidlistlen = yy.str();
	
		        // Send back the number of CIDs
	            cout << "Sending back " << cidlistlen << endl;
	            Xsend(acceptSock,(void *) cidlistlen.c_str(), cidlistlen.length(), 0);
			}
			else{
	            cerr << "Invalid Video Name: " << videoName << endl;
	            Xclose(acceptSock);
	            delete sock;
	            return NULL;
			}
        } 
        else if(isTerminationRequest(SIDReqStr)){
            clientSignaledToClose = true;
        }
		else if(isVideoSelectionRequest(SIDReqStr)){
			ostringstream oss;
			for(vector<string>::iterator it = videoList.begin(); it != videoList.end(); ++it){

				oss << *it << " ";
			}
			Xsend(acceptSock,(void *) oss.str().c_str(), oss.str().length(), 0);
		}
        else {
            // Otherwise, if the request was not about the number of chunks,
            // it must be a request for a certain chunk

            // Format of the request:   start-offset:end-offset
            // Each offset position corresponds to a CID (chunk)

            cout << "Request is for a certain chunk span" << endl;

            // Parse the Request, extract start and end offsets

            int findpos = SIDReqStr.find(":");
            // split around this position
            string prefix = "block ";
            string str = SIDReqStr.substr(prefix.length(), findpos);
            int start_offset = atoi(str.c_str()); 
            str = SIDReqStr.substr(findpos + 1);
            int end_offset = atoi(str.c_str());

            cout << "Start: " << start_offset << endl;
            cout << "End: " << end_offset << endl;

            // construct the string from CIDlist
            // return the list of CIDs, NOT including end_offset
            string requestedCIDlist = "";
            for(int i = start_offset; i < end_offset; i++){
                requestedCIDlist += CIDlist[videoName]->at(i) + " ";
            }       
            Xsend(acceptSock, (void *)requestedCIDlist.c_str(), requestedCIDlist.length(), 0);
            cout << "sending requested CID list: " << requestedCIDlist << endl;
        }
    }

    Xclose(acceptSock);
    delete sock;
	return NULL;
}



bool isNumChunksRequest(string requestStr){
	return requestStr.find("numchunks") != string::npos;
}


bool isTerminationRequest(string requestStr){
	return requestStr.find("done") != string::npos;
}


bool isVideoSelectionRequest(string requestStr){
	return requestStr.find("videos") != string::npos;
}





void help(const char *name)
{
    printf("\n%s (%s)\n", TITLE, VERSION);
    printf("usage: %s [-q] file\n", name);
    printf("where:\n");
    printf(" -q : quiet mode\n");
    printf(" file is the video file to serve up\n");
    printf("\n");
    exit(0);
}


void getConfig(int argc, char** argv)
{
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "q")) != -1) {
        switch (c) {
            case 'q':
                // turn off info messages
            VERBOSE = 0;
            break;
            case '?':
            case 'h':
            default:
                // Help Me!
            help(basename(argv[0]));
        }
    }

    //if (argc - optind != 1)
    //    help(basename(argv[0]));

    //VIDEO_NAME = argv[optind];
    //HARD-CODED SO NO LONGER USED *****
}



void readInCIDLists(){	
	for(vector<string>::iterator it = videoList.begin(); it != videoList.end(); ++it){
		string videoName = *it;
		
		// Create entry in our map of <Video Name to CID strings>
		CIDlist[videoName] = new vector<string>();
		
		string fileName = "CIDs_" + videoName + ".txt";
		ifstream infile(fileName.c_str());
		string CID_str;
		while(infile >> CID_str){
			CIDlist[videoName]->push_back(CID_str);
		}
		infile.close();
	}
}




