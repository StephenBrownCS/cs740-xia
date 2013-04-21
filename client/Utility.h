#ifndef __UTILITY_H__
#define __UTILITY_H__

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

#endif
