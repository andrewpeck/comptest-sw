#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <stdint.h>
static const uint32_t ADR_COMP_CONFIG       = 0x1;
static const uint32_t ADR_PULSE_CTRL        = 0x2;
static const uint32_t ADR_MUX1              = 0x3;
static const uint32_t ADR_MUX2              = 0x4;
static const uint32_t ADR_HALFSTRIPS        = 0x5;
static const uint32_t ADR_HALFSTRIPS_EXPECT = 0x6;
static const uint32_t ADR_HALFSTRIPS_ERRCNT = 0x7;
static const uint32_t ADR_ADC               = 0x8;
static const uint32_t ADR_DDD               = 0x9;
static const uint32_t ADR_COMPOUT_ERRCNT    = 0xA;
static const uint32_t ADR_THRESHOLDS_ERRCNT = 0xB;
static const uint32_t ADR_ACTIVE_STRIP_MASK = 0xC;
static const uint32_t ADR_FIRE_PULSE        = 0xD;
static const uint32_t ADR_DAC               = 0xE;

static const uint8_t WRITE_CMD = 0x5;
static const uint8_t READ_CMD  = 0xA;

#endif
