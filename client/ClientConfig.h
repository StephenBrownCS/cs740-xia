
#ifndef CLIENT_CONFIG_H_
#define CLIENT_CONFIG_H_

// Represents the number of chunks the fetcher will request at each RTT
// Changing this may also require the changing of REPLY_MAX_SIZE
extern const int CHUNK_WINDOW_SIZE;

// Represents the size of the buffer to allocate when awaiting a request
// from the server- if chunk window is increased (meaning more CIDs will 
// becoming back, this constant will need to be resized appropriately)
extern const int REPLY_MAX_SIZE;

// the threshold # of chunks at which the fetcher will deicde to go get more chunks
extern const int CHUNK_QUEUE_THRESHOLD_SIZE;

// Thread will continually check the queue to see if it needs to fetch more chunks
extern const double NUM_SECONDS_TO_WAIT_BETWEEN_QUEUE_THRESHOLD_CHECKING;

// When the chunkQueue is empty, and the user is waiting for a new chunk to become 
// available, this is the time we tell them to wait for
extern const double NUM_SECONDS_TO_WAIT_FOR_NEXT_CHUNK;

// Seconds for the player to wait, while the video loads up initially
// Helps get the queue populated and ready to go
extern const double SECONDS_TO_WAIT_AT_INITIAL_LOADUP;

#endif

