#include "ddd.hpp"
#include <stdexcept>

namespace DDD {
    static const uint32_t latch = 0x1 << 0;
    static const uint32_t mosi  = 0x1 << 1;
    static const uint32_t sclk  = 0x1 << 2;

    void setDelay (int delay)
    {
        if (delay < 0)
            throw std::runtime_error ("DDD Delay Set too Small");
        if (delay > 120)
            throw std::runtime_error ("DDD Delay Set too Large");

        /* ddd chip steps in units of 2ns */
        delay = delay / 2;

        struct ddd_config dddconf;
        dddconf.ch1enable = true;
        dddconf.ch2enable = true;
        dddconf.ch3enable = true;
        dddconf.ch4enable = true;

        if (delay < 16) {
            dddconf.ch1delay  = delay;
            dddconf.ch2delay  = 0;
            dddconf.ch3delay  = 0;
            dddconf.ch4delay  = 0;
        }
        else if (delay < 32) {
            dddconf.ch1delay  = 0xF;
            dddconf.ch2delay  = delay-1*0xF;
            dddconf.ch3delay  = 0;
            dddconf.ch4delay  = 0;
        }
        else if (delay < 48) {
            dddconf.ch1delay  = 0xF;
            dddconf.ch2delay  = 0xF;
            dddconf.ch3delay  = delay-2*0xF;
            dddconf.ch4delay  = 0;
        }
        else if (delay < 64) {
            dddconf.ch1delay  = 0xF;
            dddconf.ch2delay  = 0xF;
            dddconf.ch3delay  = 0xF;
            dddconf.ch4delay  = delay-3*0xF;
        }

        setDelay (dddconf);
    }

    void setDelay (ddd_config config)
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

        uint32_t status = Serial::read (adr);
        status &= ~sclk;    // CLK Low
        status &= ~mosi;    // Data Low
        status |=  latch;   // CS High
        Serial::write(adr, status);

        for (int iclk=0; iclk<20; iclk++)
        {
            status &= ~sclk;    // CLK Low
            Serial::write(adr, status);


            status &= ~mosi;
            status |=  (data >> (20-iclk)) ? mosi : 0; // Data
            Serial::write(adr, status);

            status |= sclk;    // CLK Low
            Serial::write(adr, status);
        }

        status &= ~latch;   // Latch Low
        Serial::write(adr, status);

        // need at least 10 ns here

        status |=  latch;   // Latch High
        Serial::write(adr, status);
    }
}
