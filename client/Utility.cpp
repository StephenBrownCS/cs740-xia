#include <iostream>
#include "Utility.h"

using namespace std;

static void say(const char *fmt, ...)
{
    if (VERBOSE) {
        va_list args;

        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

static void die(int ecode, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "%s: exiting\n", TITLE);
    exit(ecode);
}