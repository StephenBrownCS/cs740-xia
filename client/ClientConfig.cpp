#include "ClientConfig.h"

// going higher than 50 breaks the program
// it would appear that XRequestChunks cannot
// support > 50 chunks at a time
const int CHUNK_WINDOW_SIZE = 50;

const int REPLY_MAX_SIZE = 512;

const int CHUNK_QUEUE_THRESHOLD_SIZE = 500;

const double NUM_SECONDS_TO_WAIT_BETWEEN_QUEUE_THRESHOLD_CHECKING = 0.2;

const double NUM_SECONDS_TO_WAIT_FOR_NEXT_CHUNK = 0.2;

const double SECONDS_TO_WAIT_AT_INITIAL_LOADUP = 10.0;
