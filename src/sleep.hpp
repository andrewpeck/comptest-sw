#ifndef SLEEP_HPP
#define SLEEP_HPP
#if defined (_WIN32)

#include <windows.h>

void usleep(__int64 usec);
#else
#include <unistd.h>
#endif

#endif
