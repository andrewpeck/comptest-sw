#ifndef LCTCOMP_HPP
#define LCTCOMP_HPP
#include <stdint.h>
#include "serial.hpp"
#include "registers.hpp"
#include "adc.hpp"

namespace Comparator {

    /* Holds LCT Comparator Decoded Output */
    struct LCTpattern_t {
        uint32_t halfstrips;
        bool compout;
    };

    // Check pattern, expect vs. read
    // return number of errors
    int patternCheck (struct LCTpattern_t expect, struct LCTpattern_t read);
    void printPattern (struct LCTpattern_t pattern);
    struct LCTpattern_t patternRead ();

    uint32_t readHalfstrips();

    uint32_t readHalfstripsErrcnt();
    uint32_t readThresholdsErrcnt();
    uint32_t readCompoutErrcnt();

    void resetCounters();
    void resetHalfstripsErrcnt();
    void resetCompoutErrcnt();
    void resetThresholdsErrcnt();

    enum PKmode_t { PKMODE0, PKMODE1, PKMODE2 , PKMODE3};
    enum PKtime_t { PKTIME25, PKTIME50, PKTIME75, PKTIME100, PKTIME125, PKTIME150, PKTIME175, PKTIME200 };

    void writePeakMode (PKmode_t peakmode);
    void writePeakTime (PKtime_t peaktime);
    void writePulseWidth (int width);
    void writeBxDelay (int delay);

    void writePatternExpect (struct LCTpattern_t expect);
    struct LCTpattern_t  readPatternExpect ();
    void writeActiveStrip (int strip);
    void writeActiveStripMask (uint32_t mask);

    struct Comparator_currents_t {
        double ibias;
        double iamp;
        double ifamp;
        double ioff;
        double i3v3;
        double i5v0;
    };

    struct Comparator_currents_t readComparatorCurrents();

    void writeTriadPersist(int persist, bool persist1);
    void writeLCTReset (bool state);
    void firePulse();
    bool isPulserReady();
    void writeCompinInject(bool state);
};

#endif
