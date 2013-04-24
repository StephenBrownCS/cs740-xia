
#ifndef UTILITY_H_
#define UTILITY_H_

extern int VERBOSE;

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
