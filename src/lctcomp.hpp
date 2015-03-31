#ifndef LCTCOMP_HPP
#define LCTCOMP_HPP
#include <stdint.h>
#include "serial.hpp"
#include "registers.hpp"
#include "adc.hpp"
class Comparator {
    public:

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
        int readCompoutErrcnt();
        void resetHalfstripsErrcnt();
        void resetCompoutErrcnt();

        enum pkmode_t { PKMODE0, PKMODE1, PKMODE2 };
        enum pktime_t { T25NS, T50NS, T75NS, T100NS, T125NS, T150NS, T175NS, T200NS };

        void setPeakMode (pkmode_t peakmode);
        void setPeakTime (pktime_t peaktime);
        void setPulseWidth (int width);
        void setBxDelay (int delay);

        void setPatternExpect (struct LCTpattern_t expect);

        struct comparator_currents_t {
            float ibias;
            float iamp;
            float ifamp;
            float ioff;
            float i3v3;
            float i5v0;
        };

        struct comparator_currents_t readComparatorCurrents();

        void setTriadPersist(int persist, bool persist1);
        void setLCTReset (bool state);
        void firePulse();
        bool getPulserReady();
        void setCompinInject(bool state);



    private:
        Serial serial;
        ADC adc;
};

#endif
