#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "Utility.h"
#include "Xsocket.h"
//#include "dagaddr.hpp"

using namespace std;


const int LVL_INFO = 1;
const int LVL_DEBUG = 0;

const int MAX_SIZE_XID = 64;


bool VERBOSE = false;



void printHostInformation(){
	int sock;
	if ((sock = Xsocket(AF_XIA, SOCK_STREAM, 0)) < 0){
        die(-1, "Unable to create the listening socket\n");
    }
    
    char adBuff[MAX_SIZE_XID];
    char hidBuff[MAX_SIZE_XID];
    char fourIdBuff[MAX_SIZE_XID];
	XreadLocalHostAddr(sock, adBuff, MAX_SIZE_XID, hidBuff, MAX_SIZE_XID, fourIdBuff, MAX_SIZE_XID);
	
	string ad(adBuff);
	string hid(hidBuff);
	string fourId(fourIdBuff);
	say("Host Information:");
	say("\tAD: " + ad + "\n\tHID: " + hid + "\n\t4ID: " + fourId);
	
	Xclose(sock);
}


void say(string msg, int priorityLevel){
    if(VERBOSE || priorityLevel == LVL_INFO){
        cout << msg << endl;
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
    fprintf(stdout, "exiting\n");
    exit(ecode);
}
