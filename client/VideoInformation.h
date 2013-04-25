
#include <string>
#include <vector>

struct VideoInformation{
	// number of chunks for this video
    int numChunks;
    
    // a list of AD-HIDs
    vector<string> hosts;
};
