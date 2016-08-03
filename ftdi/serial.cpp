#include <stdio.h>
#include <stdlib.h>

#include "sleep.hpp"

#include <signal.h>

#include "serial.hpp"
#include "registers.hpp"

// TODO: Software command to flush the serial
//       state machine (force it to return idle)
//       basically... need to send 4? bytes of data,
//       none of which should begin with 0x5 or 0xA

namespace Serial {
    const int vid = 0x0403;
    const int pid = 0x6014;

    #ifdef _WIN32
        #define d2xx
    #endif

    #ifdef emulate
        #include "serial_emulator.inc"
    #elif defined d2xx
        #include "serial_d2xx.inc"
    #else
        #include "serial_libftdi.inc"
    #endif
}
