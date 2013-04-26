#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "Xsocket.h"
#include "Utility.h"

using namespace std;

bool VERBOSE = true;


void printHostInformation(){
	int sock;
	if ((sock = Xsocket(AF_XIA, SOCK_STREAM, 0)) < 0){
        die(-1, "Unable to create the listening socket\n");
    }
    
    char adBuff[1024];
    char hidBuff[1024];
    char fourIdBuff[1024];
	XreadLocalHostAddr(sock, adBuff, 1024, hidBuff, 1024, fourIdBuff, 1024);
	
	string ad(adBuff);
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

void die(int ecode, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "exiting\n");
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

void thread_sleep(double numSeconds){
    const int NUM_NANOSECONDS_IN_A_SECOND = 1000000000;
    
    int wholeIntegerPart = static_cast<int>(floor(numSeconds));
    double fractionalPart = numSeconds  - wholeIntegerPart;
    
    struct timespec tim, tim2;
    tim.tv_sec = wholeIntegerPart;
    tim.tv_nsec = static_cast<int>(floor(fractionalPart * NUM_NANOSECONDS_IN_A_SECOND));

    if(nanosleep(&tim, &tim2) < 0 ){
        cerr << "Unable to sleep" << endl;
        exit(-1);
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
