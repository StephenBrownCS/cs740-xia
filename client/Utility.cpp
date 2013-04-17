#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include "Utility.h"

using namespace std;

const bool VERBOSE = true;
const char* const TITLE = "";

void say(const char *fmt, ...)
{
    if (VERBOSE) {
        va_list args;

        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

void die(int ecode, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "%s: exiting\n", TITLE);
    exit(ecode);
}
