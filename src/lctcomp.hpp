#ifndef LCTCOMP_HPP
#define LCTCOMP_HPP
#include <stdint.h>
#include "serial.hpp"
#include "registers.hpp"
#include "adc.hpp"
class Comparator {
    public:

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

        int readHalfstripsErrcnt();
        int readThresholdsErrcnt();
        int readCompoutErrcnt();

        void resetHalfstripsErrcnt();
        void resetCompoutErrcnt();
        void resetThresholdsErrcnt();

        enum PKmode_t { PKMODE0, PKMODE1, PKMODE2 };
        enum PKtime_t { PKTIME25, PKTIME50, PKTIME75, PKTIME100, PKTIME125, PKTIME150, PKTIME175, PKTIME200 };

        void writePeakMode (PKmode_t peakmode);
        void writePeakTime (PKtime_t peaktime);
        void writePulseWidth (int width);
        void writeBxDelay (int delay);

        void writePatternExpect (struct LCTpattern_t expect);

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

    private:
        Serial serial;
        ADC adc;
};

#endif
