
#ifndef UTILITY_H_
#define UTILITY_H_

#include "Xsocket.h"

extern int VERBOSE;

sockaddr_x* getOwnDag();
string getOwnAd();
string getOwnHid();
string extractDagAd(sockaddr_x dagStr);
string extractDagHid(sockaddr_x dagStr);

/*
** write the message to stdout unless in quiet mode
*/
    void say(const char *fmt, ...);

/*
** always write the message to stdout
*/
    void warn(const char *fmt, ...);


/*
** write the message to stdout, and exit the app
*/
    void die(int ecode, const char *fmt, ...);

#endif
