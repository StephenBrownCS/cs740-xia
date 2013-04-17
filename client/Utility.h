#ifndef __UTILITY_H__
#define __UTILITY_H__

/*
** write the message to stdout unless in quiet mode
*/
static void say(const char *fmt, ...);

/*
** write the message to stdout, and exit the app
*/
static void die(int ecode, const char *fmt, ...);

#endif
