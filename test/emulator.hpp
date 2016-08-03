#ifndef EMULATOR_HPP
#define EMULATOR_HPP
#include <random>

namespace Emulator
{
    void write (uint8_t adr, uint32_t write_data);
    uint32_t read (uint8_t adr);
};
#endif
