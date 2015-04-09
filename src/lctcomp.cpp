#include "lctcomp.hpp"
#include "cstdio"

// Read pattern from comparators
struct Comparator::LCTpattern_t Comparator::patternRead ()
{
    LCTpattern_t pat;
    pat.halfstrips = serial.read (ADR_HALFSTRIPS);
    pat.compout    = 0x1 & (serial.read (ADR_PULSE_CTRL) >> 20);
    return (pat);
}

void Comparator::printPattern (struct Comparator::LCTpattern_t pattern)
{
    for (int i=31; i>=0; i--)
        printf("%i", (pattern.halfstrips >> i) & 0x1);
    printf(" o%i\n", (pattern.compout) & 0x1);
}

// Check pattern, expect vs. read
int Comparator::patternCheck (struct Comparator::LCTpattern_t expect, struct
        Comparator::LCTpattern_t read)
{
    uint32_t diff = expect.halfstrips ^ read.halfstrips;
    int num_errors = __builtin_popcount (diff);

    if (expect.compout != read.compout)
        num_errors += 1;

    return num_errors;
}

int Comparator::readHalfstripsErrcnt()
{
    int errcnt = serial.read(ADR_HALFSTRIPS_ERRCNT);
    return errcnt;
}

int Comparator::readCompoutErrcnt()
{
    int errcnt = serial.read(ADR_COMPOUT_ERRCNT);
    return errcnt;
}

void Comparator::resetHalfstripsErrcnt()
{
    uint8_t adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read (adr);
    status |= 0x1 << 8; //halfstrips_errcnt
    serial.write(adr, status);
    status &= ~(0x1 << 8);
    serial.write(adr, status);
}

void Comparator::resetCompoutErrcnt()
{
    uint8_t adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read (adr);
    status |= 0x1 << 9; //halfstrips_errcnt
    serial.write(adr, status);
    status &= ~(0x1 << 9);
    serial.write(adr, status);
}

void Comparator::writePeakMode (PKmode_t peakmode)
{
    uint8_t adr = ADR_COMP_CONFIG;
    uint32_t pkmode_mask = ~(0x3 << 3);
    uint32_t status = serial.read(adr);
    status &= pkmode_mask;
    status |= peakmode;
    serial.write(adr, status);
}

void Comparator::writePeakTime (PKtime_t peaktime)
{
    uint8_t adr = ADR_COMP_CONFIG;
    uint32_t pktime_mask = ~(0x7 << 0);
    uint32_t status = serial.read(adr);
    status &= pktime_mask;
    status |= peaktime;
    serial.write(adr, status);
}

void Comparator::writePulseWidth (int width)
{
    uint8_t adr = ADR_COMP_CONFIG;
    width &= 0xF; // max 4 bits
    uint32_t pulsewidth_mask = ~(0xF << 1);
    uint32_t status = serial.read(adr);
    status &= pulsewidth_mask;
    status |= width;
    serial.write (adr, status);
}

void Comparator::writeBxDelay (int delay)
{
    uint8_t adr = ADR_COMP_CONFIG;
    delay &= 0x7; // max 3 bits
    uint32_t bxdelay_mask = ~(0x7 << 11);
    uint32_t status = serial.read(adr);
    status &= bxdelay_mask;
    status |= delay;
    serial.write(adr, status);
}

void Comparator::writeTriadPersist(int persist, bool persist1)
{
    uint8_t adr = ADR_COMP_CONFIG;
    persist  &= 0x7;  //max 3 bits
    persist1 &= 0x1;  //max 1 bits
    uint32_t persist_mask =  ~(0x7 << 15);
    uint32_t persist1_mask = ~(0x1 << 19);

    uint32_t status = serial.read(adr);
    status &= persist_mask;
    status &= persist1_mask;

    status |= persist;
    status |= persist1;

    serial.write(adr, status);
}

void Comparator::writePatternExpect (struct Comparator::LCTpattern_t expect)
{
    uint8_t adr = ADR_HALFSTRIPS_EXPECT;
    serial.write(adr, expect.halfstrips);

    adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read(adr);
    status &= ~(0x1 << 14);
    status |= 0x1 & expect.compout;
    serial.write(adr, status);
}

void Comparator::writeLCTReset (bool state)
{
    uint8_t adr = ADR_COMP_CONFIG;
    state &= 0x1; // max 1 bits
    uint32_t status = serial.read(adr);
    status &= 0x1 << 5;
    status |= 0x1 & (state << 5);
    serial.write(adr, status);
}

void Comparator::firePulse()
{
    uint8_t adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read (adr);
    status |= 0x1 << 0;
    serial.write(adr, status);
    status &= ~(0x1 << 8);
    serial.write(adr, status);
}

bool Comparator::isPulserReady()
{
    uint8_t adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read (adr);
    bool ready = 0x1 & (status >> 21);
    return ready;
}

void Comparator::writeCompinInject(bool state)
{
    uint8_t adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read (adr);
    status &= ~(0x1 << 10);
    status |= state;
    serial.write(adr, status);
}

struct Comparator::Comparator_currents_t Comparator::readComparatorCurrents()
{
    struct Comparator_currents_t icomp;
    struct Comparator_currents_t iscaler;

    iscaler.ibias = .00010f;
    iscaler.iamp  = .00010f;
    iscaler.ifamp = .00010f;
    iscaler.ioff  = .00010f;
    iscaler.i3v3  = .02;
    iscaler.i5v0  = .02;

    icomp.ibias = 1000000. * iscaler.ibias * adc.readVoltage (0); //microamps
    icomp.ioff  = 1000000. * iscaler.ioff  * adc.readVoltage (1); //microamps
    icomp.iamp  = 1000000. * iscaler.iamp  * adc.readVoltage (2); //microamps
    icomp.ifamp = 1000000. * iscaler.ifamp * adc.readVoltage (3); //microamps
    icomp.i3v3  = 1000. * iscaler.i3v3  * adc.readVoltage (4); //milliamps
    icomp.i5v0  = 1000. * iscaler.i5v0  * adc.readVoltage (5); //milliamps

    return icomp;
}
