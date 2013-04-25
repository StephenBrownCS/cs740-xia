#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "Utility.h"

using namespace std;

int VERBOSE = 1;

sockaddr_x* getOwnDag(){
	struct addrinfo *addrInfo;
	if (Xgetaddrinfo(NULL, SID_VIDEO, NULL, &addrInfo) < 0)
	    die(-1, "Unable to create the local dag\n");

	sockaddr_x* dag = (sockaddr_x*) addrInfo->ai_addr;
	return dag;
}

string getOwnAd(){
	sockaddr_x* dag = getOwnDag();
	string ad = extractDagAd(*dag);
	
	//TODO may need to free dag here?
	return ad;
}

string getOwnHid(){
	sockaddr_x* dag = getOwnDag();
	string hid = extractDagHid(*dag);
	//TODO may ned to free dag here
	return hid;
}


string extractDagAd(sockaddr_x dagStr){
	Graph g(&dagStr);
	string dag = g.dag_string();
	
	cout << "Dag String: " << dag << endl;
	
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
