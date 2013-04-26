#ifndef __UTILITY_H__
#define __UTILITY_H__

extern bool VERBOSE;


void printHostInformation();

/*
** write the message to stdout unless in quiet mode
*/
void say(const char *fmt, ...);

/*
** write the message to stdout, and exit the app
*/
void die(int ecode, const char *fmt, ...);

/*
** Sends the cmd string trhough the socket
** If unsuccessful, closes the socket, and quits the program
*/
int sendCmd(int sock, const char *cmd);

/** 
 *  Sleeps for the number of seconds (including fractional seconds)
*/
void thread_sleep(double numSeconds);

/**
 * Extract the Autonomous Domain ID from the dag
 * Returns the string, including the AD: prefix
*/
string extractDagAd(sockaddr_x dagStr);

/**
 * Extract the Host ID from the dag
 * Returns the string, including the HID: prefix
*/
string extractDagHid(sockaddr_x dagStr);

#endif
