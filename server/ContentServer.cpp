#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
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

const int PERIOD_TO_SLEEP = 40;

/*
** upload the video file as content chunks
*/
    int uploadContent(const char *fname);


//***************************************************************************
//******                    MAIN METHOD                                 *****
//***************************************************************************

int main(int argc, char *argv[])
{
    printHostInformation();
    
    // put the video file into the content cache
    if (uploadContent(VIDEO_NAME.c_str()) != 0){
        die(-1, "Unable to upload the video %s\n", VIDEO_NAME.c_str());
    }
    return 0;
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

	//TODO: Remove
	ofstream outfile("CIDs_BigBuckBunny.txt");
   	for (int i = 0; i < count; i++) {
        string CID = "CID:";
        CID += info[i].cid;
		outfile << CID << endl;
    }
	outfile.close();

    // Sleep for some period of time, then clear the content cache
    //sleep(PERIOD_TO_SLEEP);
    /*
    cout << "Removing all CIDs" << endl;
    for (int i = 0; i < count; i++) {
        XremoveChunk(ctx, info[i].cid);
    }
*/

    XfreeChunkInfo(info);

    // close the connection to the cache slice, but becase it is set to retain,
    // the content will stay available in the cache
    XfreeCacheSlice(ctx);
    
    return 0;
}



