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

void Comparator::setPeakMode (pkmode_t peakmode)
{
    uint8_t adr = ADR_COMP_CONFIG;
    uint32_t pkmode_mask = ~(0x3 << 3);
    uint32_t status = serial.read(adr);
    status &= pkmode_mask;
    status |= peakmode;
    serial.write(adr, status);
}

void Comparator::setPeakTime (pktime_t peaktime)
{
    uint8_t adr = ADR_COMP_CONFIG;
    uint32_t pktime_mask = ~(0x7 << 0);
    uint32_t status = serial.read(adr);
    status &= pktime_mask;
    status |= peaktime;
    serial.write(adr, status);
}

void Comparator::setPulseWidth (int width)
{
    uint8_t adr = ADR_COMP_CONFIG;
    width &= 0xF; // max 4 bits
    uint32_t pulsewidth_mask = ~(0xF << 1);
    uint32_t status = serial.read(adr);
    status &= pulsewidth_mask;
    status |= width;
    serial.write (adr, status);
}

void Comparator::setBxDelay (int delay)
{
    uint8_t adr = ADR_COMP_CONFIG;
    delay &= 0x7; // max 3 bits
    uint32_t bxdelay_mask = ~(0x7 << 11);
    uint32_t status = serial.read(adr);
    status &= bxdelay_mask;
    status |= delay;
    serial.write(adr, status);
}

void Comparator::setTriadPersist(int persist, bool persist1)
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

void Comparator::setPatternExpect (struct Comparator::LCTpattern_t expect)
{
    uint8_t adr = ADR_HALFSTRIPS_EXPECT;
    serial.write(adr, expect.halfstrips);

    adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read(adr);
    status &= ~(0x1 << 14);
    status |= 0x1 & expect.compout;
    serial.write(adr, status);
}

void Comparator::setLCTReset (bool state)
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

bool Comparator::getPulserReady()
{
    uint8_t adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read (adr);
    bool ready = 0x1 & (status >> 21);
    return ready;
}

void Comparator::setCompinInject(bool state)
{
    uint8_t adr = ADR_PULSE_CTRL;
    uint32_t status = serial.read (adr);
    status &= ~(0x1 << 10);
    status |= state;
    serial.write(adr, status);
}

struct Comparator::comparator_currents_t Comparator::readComparatorCurrents()
{
    struct comparator_currents_t icomp;
    struct comparator_currents_t iscaler;

    iscaler.ibias = 1.0f;
    iscaler.iamp  = 1.0f;
    iscaler.ifamp = 1.0f;
    iscaler.ioff  = 1.0f;
    iscaler.i3v3  = 1.0f;
    iscaler.i5v0  = 1.0f;

    icomp.ibias = iscaler.ibias * adc.readVoltage (0);
    icomp.ioff  = iscaler.ioff  * adc.readVoltage (1);
    icomp.iamp  = iscaler.iamp  * adc.readVoltage (2);
    icomp.ifamp = iscaler.ifamp * adc.readVoltage (3);
    icomp.i3v3  = iscaler.i3v3  * adc.readVoltage (4);
    icomp.i5v0  = iscaler.i5v0  * adc.readVoltage (5);

    return icomp;
}
