
#ifndef VIDEO_INFORMATION_H_
#define VIDEO_INFORMATION_H_

#include <string>
#include <deque>

class ServerLocation{
    std::string ad;
    std::string hid;
public:
    ServerLocation(std::string protoDag);
    
    /**
     * Returns the AD String without the AD: prefix
    */
    std::string getAd(){
        return ad;
    }
   
   
    /**
     * Returns the Hid String without the HID: Prefix
    */ 
    std::string getHid(){
        return hid;
    }
};



class VideoInformation{
public:
	// number of chunks for this video
    const int numChunks;
    
    VideoInformation(int numChunks_): numChunks(numChunks_) {}
    
    void addServerLocation(ServerLocation serverLocation);
    
    int getNumServerLocations();
    
    ServerLocation getServerLocation(int index);
    
    void rotateServerLocations();

    void printServerLocations();
    
private:
    // a list of AD-HIDs
    std::deque<ServerLocation> serverLocations;
};

#endif

