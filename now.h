#ifndef NOW_H
#define NOW_H

#ifdef _WIN32
#include "timeutils.inc"
#else
#include <sys/time.h>
#endif

#include <iostream>

std::string now()
{
    /* Logging */
    char datestr [80];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(datestr, sizeof(datestr)-1, "%Y%m%d-%H%M%S", t);

    timeval curTime;
    gettimeofday(&curTime, NULL);
    int millis = curTime.tv_usec / 1000;

    sprintf(datestr, "%s%03i", datestr, millis);

    std::string filename = datestr;
    return filename;
}

#endif /* NOW_H */
