#include "lctcomp.hpp"
#include <cstdio>
#include <cassert>

namespace Comparator {
    // Read pattern from comparators
    struct LCTpattern_t patternRead ()
    {
        LCTpattern_t pat;
        pat.halfstrips = Serial::read (ADR_HALFSTRIPS);
        pat.compout    = 0x1 & (Serial::read (ADR_PULSE_CTRL) >> 20);
        return (pat);
    }

    void printPattern (struct LCTpattern_t pattern)
    {
        for (int i=31; i>=0; i--)
            printf("%i", (pattern.halfstrips >> i) & 0x1);
        printf(" o%i\n", (pattern.compout) & 0x1);
    }

    // Check pattern, expect vs. read
    int patternCheck (struct LCTpattern_t expect, struct
            LCTpattern_t read)
    {
#ifdef _WIN32
#  include <intrin.h>
#  define __builtin_popcount __popcnt
#endif
        uint32_t diff = expect.halfstrips ^ read.halfstrips;
        int num_errors = __builtin_popcount (diff);

        if (expect.compout != read.compout)
            num_errors += 1;

        return num_errors;
    }

    uint32_t readHalfstrips()
    {
        return Serial::read(ADR_HALFSTRIPS);
    }

    uint32_t readHalfstripsErrcnt()
    {
        return Serial::read(ADR_HALFSTRIPS_ERRCNT);
    }

    uint32_t readThresholdsErrcnt()
    {
        return Serial::read(ADR_THRESHOLDS_ERRCNT);
    }

    uint32_t readCompoutErrcnt()
    {
        return Serial::read(ADR_COMPOUT_ERRCNT);
    }

    void resetCounters()
    {
        uint8_t adr = ADR_FIRE_PULSE;
        uint32_t status = 0x7 << 1;
        Serial::write(adr, status);
        status = 0;
        Serial::write(adr, status);
    }

    void resetHalfstripsErrcnt()
    {
        uint8_t adr = ADR_FIRE_PULSE;
        uint32_t status = 0x1 << 1; //halfstrips_errcnt
        Serial::write(adr, status);
        status = 0;
        Serial::write(adr, status);
    }

    void resetCompoutErrcnt()
    {
        uint8_t adr = ADR_FIRE_PULSE;
        uint32_t status = 0x1 << 2;
        Serial::write(adr, status);
        status = 0;
        Serial::write(adr, status);
    }

    void resetThresholdsErrcnt()
    {
        uint8_t adr = ADR_FIRE_PULSE;
        uint32_t status = 0x1 << 3;
        Serial::write(adr, status);
        status = 0;
        Serial::write(adr, status);
    }

    void writePeakMode (PKmode_t peakmode)
    {
        uint8_t adr = ADR_COMP_CONFIG;
        uint32_t pkmode_mask = ~(0x3 << 3);
        uint32_t status = Serial::read(adr);
        status &= pkmode_mask;
        status |= peakmode << 3;
        Serial::write(adr, status);
    }

    void writePeakTime (PKtime_t peaktime)
    {
        uint8_t adr = ADR_COMP_CONFIG;
        uint32_t pktime_mask = ~(0x7 << 0);
        uint32_t status = Serial::read(adr);
        status &= pktime_mask;
        status |= peaktime;
        Serial::write(adr, status);
    }

    void writePulseWidth (int width)
    {
        assert (width>0);
        assert (width<16);

        uint8_t adr = ADR_PULSE_CTRL;
        width &= 0xF; // max 4 bits
        uint32_t pulsewidth_mask = ~(0xF << 1);
        uint32_t status = Serial::read(adr);
        status &= pulsewidth_mask;
        status |= width;
        Serial::write (adr, status);
    }

    void writeBxDelay (int delay)
    {
        uint8_t adr = ADR_PULSE_CTRL;
        delay &= 0x0f; // max 4 bits
        uint32_t status = Serial::read(adr);
        status &= ~(0xf   << 11);
        status |=  (delay << 11);
        Serial::write(adr, status);
    }

    void writeTriadPersist(int persist, bool persist1)
    {
        uint8_t adr = ADR_PULSE_CTRL;
        persist  &= 0x7;  //max 3 bits
        persist1 &= 0x1;  //max 1 bits
        uint32_t persist_mask =  ~(0x7 << 16);
        uint32_t persist1_mask = ~(0x1 << 20);

        uint32_t status = Serial::read(adr);
        status &= persist_mask;
        status &= persist1_mask;

        status |= persist;
        status |= persist1;

        Serial::write(adr, status);
    }

    void writePatternExpect (struct LCTpattern_t expect)
    {
        /* Halfstrips Expect*/
        uint8_t adr = ADR_HALFSTRIPS_EXPECT;
        Serial::write(adr, expect.halfstrips);

        /* Compout Expect */
        adr = ADR_PULSE_CTRL;
        uint32_t status = Serial::read(adr);
        status &= ~(0x1 << 15);
        status |= 0x1 & expect.compout;
        Serial::write(adr, status);
    }

    struct LCTpattern_t readPatternExpect ()
    {
        struct LCTpattern_t expect;

        /* Halfstrips Expect*/
        uint8_t adr = ADR_HALFSTRIPS_EXPECT;
        expect.halfstrips = Serial::read(adr);

        /* Compout Expect */
        adr = ADR_PULSE_CTRL;
        uint32_t status = Serial::read(adr);
        status = 0x1 & (status >> 15);
        expect.compout = status;

        return expect;
    }

    void writeActiveStrip (int strip)
    {
        uint32_t mask = 0;
        mask |= 0x1 << (strip);
        mask |= 0x1 << (strip+1);
        writeActiveStripMask(mask);
    }

    void writeActiveStripMask (uint32_t mask)
    {
        uint8_t  adr = ADR_ACTIVE_STRIP_MASK;
        Serial::write(adr, mask & 0xFFFF);
    }

    void writeLCTReset (bool state)
    {
        uint8_t adr = ADR_COMP_CONFIG;
        state &= 0x1; // max 1 bits
        uint32_t status = Serial::read(adr);
        status &= ~(0x1   << 5);
        status |=  (state << 5);
        Serial::write(adr, status);
    }

    void firePulse()
    {
        uint8_t adr = ADR_FIRE_PULSE;
        uint32_t status = 0x1;
        Serial::write(adr, status);
        status = 0x0;
        Serial::write(adr, status);
    }

    bool isPulserReady()
    {
        uint8_t adr = ADR_PULSE_CTRL;
        uint32_t status = Serial::read (adr);
        bool ready = 0x1 & (status >> 21);
        return ready;
    }

    void writeCompinInject(bool state)
    {
        uint8_t adr = ADR_PULSE_CTRL;
        uint32_t status = Serial::read (adr);
        status &= ~(0x1 << 10);
        status |= ((0x1&state) << 10);
        Serial::write(adr, status);
    }

    struct Comparator_currents_t readComparatorCurrents()
    {
        struct Comparator_currents_t icomp;
        struct Comparator_currents_t iscaler;

        iscaler.ibias = .0000823;
        iscaler.iamp  = .0000430;
        iscaler.ifamp = .0000430;
        iscaler.ioff  = .00000746;
        iscaler.i3v3  =  1.0/9375.0;
        iscaler.i5v0  =  1.0/(970*0.033);

        icomp.ibias = 1000000. * iscaler.ibias * ADC::readVoltage (0); //microamps
        icomp.ioff  = 1000000. * iscaler.ioff  * ADC::readVoltage (1); //microamps
        icomp.iamp  = 1000000. * iscaler.iamp  * ADC::readVoltage (2); //microamps
        icomp.ifamp = 1000000. * iscaler.ifamp * ADC::readVoltage (3); //microamps
        icomp.i3v3  = 1000000. * iscaler.i3v3  * ADC::readVoltage (4); //microamps
        icomp.i5v0  = 1000.    * iscaler.i5v0  * ADC::readVoltage (5); //milliamps

        return icomp;
    }
}
