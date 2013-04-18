#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include "Xsocket.h"
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

int sendCmd(int sock, const char *cmd)
{
    int n;

    if ((n = Xsend(sock, cmd,  strlen(cmd), 0)) < 0) {
        Xclose(sock);
         die(-1, "Unable to communicate with the server\n");
    }

    return n;
}
