#include "ddd.hpp"

void DDD::setDelay (ddd_config config)
{
    int adr = ADR_DDD;

    uint32_t data = 0;
    data |= 0x1 & config.ch4enable << 0;
    data |= 0x1 & config.ch3enable << 1;
    data |= 0x1 & config.ch2enable << 2;
    data |= 0x1 & config.ch1enable << 3;
    data |= 0xF & config.ch1delay  << 4;
    data |= 0xF & config.ch2delay  << 8;
    data |= 0xF & config.ch3delay  << 12;
    data |= 0xF & config.ch4delay  << 16;

    uint32_t status = serial.read (adr);
    status &= ~sclk;    // CLK Low
    status &= ~mosi;    // Data Low
    status |=  latch;   // CS High
    serial.write(adr, status);

    for (int iclk=0; iclk<20; iclk++)
    {
        status &= ~sclk;    // CLK Low
        serial.write(adr, status);


        status &= ~mosi;
        status |=  (data >> (20-iclk)) ? mosi : 0; // Data
        serial.write(adr, status);

        status |= sclk;    // CLK Low
        serial.write(adr, status);
    }

    status &= ~latch;   // Latch Low
    serial.write(adr, status);

    // need at least 10 ns here

    status |=  latch;   // Latch High
    serial.write(adr, status);
}
