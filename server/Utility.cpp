#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "Utility.h"
#include "Xsocket.h"
//#include "dagaddr.hpp"

using namespace std;

int VERBOSE = 1;



void printHostInformation(){
	int sock;
	if ((sock = Xsocket(AF_XIA, SOCK_STREAM, 0)) < 0){
        die(-1, "Unable to create the listening socket\n");
    }
    
    char adBuff[1024];
    char hidBuff[1024];
    char fourIdBuff[1024];
	XreadLocalHostAddr(sock, adBuff, 1024, hidBuff, 1024, fourIdBuff, 1024);
	

	cout << "AD: " << adBuff << endl;
	cout << "HID: " << hidBuff << endl;
	cout << "4ID: " << fourIdBuff << endl;
	
	Xclose(sock);
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
    fprintf(stdout, "exiting\n");
    exit(ecode);
}
