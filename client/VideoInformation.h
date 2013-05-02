
#ifndef VIDEO_INFORMATION_H_
#define VIDEO_INFORMATION_H_

#include <string>
#include <deque>

// Represents a ServerLocation's XID
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


// Represents the information about a video
// Includes the number of chunks, as well as a list of content server XIDs
class VideoInformation{
public:    
    VideoInformation(int numChunks_): numChunks(numChunks_) {}  
    
    /**
     *
     */
    int getNumChunks(){
        return numChunks;
    }
    
    // Adds a server location to the list of locations
    void addServerLocation(ServerLocation serverLocation);
    
    // Returns the number of known server locations for this video
    int getNumServerLocations();
    
    // Returns a specific server location
    ServerLocation getServerLocation(int index);
    
    // Rotates the server list by one. The first server in the list will 
    // be rotated to the bottom of the list.
    void rotateServerLocations();

    // Prints server locations to screen
    void printServerLocations();
    
private:
	// number of chunks for this video
    const int numChunks;
    
    // a list of AD-HIDs
    std::deque<ServerLocation> serverLocations;
};

#endif

