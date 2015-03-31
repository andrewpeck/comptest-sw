#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <stdint.h>
static const uint32_t ADR_COMP_CONFIG       = 0x01;
static const uint32_t ADR_PULSE_CTRL        = 0x02;
static const uint32_t ADR_MUX1              = 0x03;
static const uint32_t ADR_MUX2              = 0x04;
static const uint32_t ADR_HALFSTRIPS        = 0x05;
static const uint32_t ADR_HALFSTRIPS_EXPECT = 0x06;
static const uint32_t ADR_HALFSTRIPS_ERRCNT = 0x07;
static const uint32_t ADR_COMPOUT_ERRCNT    = 0x0A;
static const uint32_t ADR_ADC               = 0x08;
static const uint32_t ADR_DDD               = 0x09;

static const uint8_t WRITE_CMD = 0x5;
static const uint8_t READ_CMD  = 0xA;

#endif
