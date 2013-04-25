
#ifndef VIDEO_INFORMATION_H_
#define VIDEO_INFORMATION_H_

#include <string>
#include <vector>

struct VideoInformation{
	// number of chunks for this video
    int numChunks;
    
    // a list of AD-HIDs
    std::vector<std::string> hosts;
};

#endif

