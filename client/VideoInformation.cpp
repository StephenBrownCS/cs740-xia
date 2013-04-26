
#include "VideoInformation.h"
#include "Utility.h"

using namespace std;

ServerLocation::ServerLocation(String protoDag){
    {
        int beginPos = protoDag.find("AD:");
        int endPos = protoDag.find(" ", beginPos);
        ad = protoDag.substr(beginPos, endPos - beginPos);
	}
	
	{
        int beginPos = protoDag.find("HID:");
        int endPos = protoDag.find(" ", beginPos);
        hid = protoDag.substr(beginPos, endPos - beginPos);
    }
}

void VideoInformation::addServerLocation(ServerLocation serverLocation){
    serverLocations.push_back(serverLocation);
}

int VideoInformation::getNumServerLocations(){
    return serverLocations.size();
}

ServerLocation VideoInformation::getServerLocation(int index){
    return serverLocations[index];
}

