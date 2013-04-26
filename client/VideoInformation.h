
#ifndef VIDEO_INFORMATION_H_
#define VIDEO_INFORMATION_H_

#include <string>
#include <vector>

class ServerLocation{
    std::string ad;
    std::string hid;
public:
    ServerLocation(String protoDag);
    
    std::tring getAd(){
        return ad;
    }
    
    std::string getHid(){
        return Hid;
    }
};



class VideoInformation{
public:
	// number of chunks for this video
    const int numChunks;
    
    VideoInformation(numChunks_): numChunks(numChunks_) {}
    
    void addServerLocation(ServerLocation serverLocation);
    
    int getNumServerLocations();
    
    ServerLocation getServerLocation(int index);
    
private:
    // a list of AD-HIDs
    std::vector<ServerLocation> serverLocations;
};

#endif

