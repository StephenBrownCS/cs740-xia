
/*
** notes: (may not be up to date)
**
** Runs for 1.5 min video, may also work with lower resolution video for bit longer
**
** seems to have some issue currently ... for multiple clients
**  so first trying to do iterative with hacky way
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <sstream>
#include <iostream>
#include "Xsocket.h"

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
int VERBOSE = 1;
string VIDEO_NAME = "../../xia-core/applications/demo/web_demo/resources/video.ogv";
//string VIDEO_NAME = "../../small.ogv";

// Container of CIDs
vector<string> CIDlist;


/*
** display cmd line options and exit
*/
    void help(const char *name);

/*
** configure the app
*/
    void getConfig(int argc, char** argv);

/*
** simple code to create a formatted DAG
**
** The dag should be free'd by the calling code when no longer needed
*/
char *createDAG(const char *ad, const char *host, const char *id);

/*
** write the message to stdout unless in quiet mode
*/
    void say(const char *fmt, ...);

/*
** always write the message to stdout
*/
    void warn(const char *fmt, ...);


/*
** write the message to stdout, and exit the app
*/
    void die(int ecode, const char *fmt, ...);


/*
** upload the video file as content chunks
*/
    int uploadContent(const char *fname);


/*
** handle the request from the client and return the requested data
*/
    void *processRequest (void *socketid);




//***************************************************************************
//******                    MAIN METHOD                                 *****
//***************************************************************************

int main(int argc, char *argv[])
{
    int sock;
    //char myAD[1024]; 
    //char myHID[1024];   

    getConfig(argc, argv);

    // put the video file into the content cache
    if (uploadContent(VIDEO_NAME.c_str()) != 0){
        die(-1, "Unable to upload the video %s\n", VIDEO_NAME.c_str());
    }

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


    bool clientSignaledToClose = false;

    while(!clientSignaledToClose){
        char SIDReq[1024];
        memset(SIDReq, 0, sizeof(SIDReq));

        //Receive packet
        say("Receiving packet...\n");
        if ((n = Xrecv(acceptSock, SIDReq, sizeof(SIDReq), 0)) <= 0) {
            cout << "Xrecv failed!" << endl;
            Xclose(acceptSock);
            delete sock;
            pthread_exit(NULL);
            return NULL;
        }

        string SIDReqStr(SIDReq);
        cout << "Got request: " << SIDReqStr << endl;
        // if the request is about number of chunks return number of chunks
        // since this is first time, you would return along with header
        unsigned int found = SIDReqStr.find("numchunks");

        // If Request contains "numchunks", return number of CID's.
        if(found != string::npos){
            cout << " Request asks for number of chunks " << endl;
            stringstream yy;
            yy << CIDlist.size();
            string cidlistlen = yy.str();

            // Send back the number of CIDs
            cout << "Sending back " << cidlistlen << endl;
            Xsend(acceptSock,(void *) cidlistlen.c_str(), cidlistlen.length(), 0);
        } 
        else if(SIDReqStr.find("done") != string::npos){
            clientSignaledToClose = true;
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
                requestedCIDlist += CIDlist[i] + " ";
            }       
            Xsend(acceptSock, (void *)requestedCIDlist.c_str(), requestedCIDlist.length(), 0);
            cout << "sending requested CID list: " << requestedCIDlist << endl;
        }
    }

    Xclose(acceptSock);
    delete sock;
    pthread_exit(NULL);
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


char *createDAG(const char *ad, const char *host, const char *id)
{
    int len = snprintf(NULL, 0, DAG, ad, host, id) + 1;

    char * dag = (char*)malloc(len);
    sprintf(dag, DAG, ad, host, id);
    return dag;
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


void warn(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

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


int uploadContent(const char *fname)
{
    int count;

    say("loading video file: %s\n", fname);
    cout << "Allocating cache slice" << endl;
    ChunkContext *ctx = XallocCacheSlice(POLICY_DEFAULT, 0, 20000000);
    if (ctx == NULL)
        die(-2, "Unable to initilize the chunking system\n");

    cout << "Putting the file..." << endl;
    ChunkInfo *info;
    if ((count = XputFile(ctx, fname, CHUNKSIZE, &info)) < 0)
        die(-3, "unable to process the video file\n");

    say("put %d chunks\n", count);

    for (int i = 0; i < count; i++) {
        string CID = "CID:";
        CID += info[i].cid;
        CIDlist.push_back(CID);
    }

    XfreeChunkInfo(info);

    // close the connection to the cache slice, but becase it is set to retain,
    // the content will stay available in the cache
    XfreeCacheSlice(ctx);
    return 0;
}




