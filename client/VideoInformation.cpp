
#include <iostream>
#include "VideoInformation.h"
#include "Utility.h"

using namespace std;

ServerLocation::ServerLocation(string protoDag){
    {
        int beginPos = protoDag.find("AD:") + 3;
        int endPos = protoDag.find(" ", beginPos);
        ad = protoDag.substr(beginPos, endPos - beginPos);
	}
	
	{
        int beginPos = protoDag.find("HID:") + 4;
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

void VideoInformation::printServerLocations(){
    for(deque<ServerLocation>::iterator it = serverLocations.begin(); it != serverLocations.end(); ++it){
        ServerLocation & location = *it;
        cout << location.getAd() << " " << location.getHid() << endl;
    }
}

void VideoInformation::rotateServerLocations(){
    ServerLocation temp = serverLocations.front();
    serverLocations.pop_front();
    serverLocations.push_back(temp);
}

ServerLocation VideoInformation::getServerLocation(int index){
    return serverLocations[index];
}
