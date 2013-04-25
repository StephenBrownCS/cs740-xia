
#ifndef UTILITY_H_
#define UTILITY_H_

#include <iostream>


extern int VERBOSE;

void printHostInformation();

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
