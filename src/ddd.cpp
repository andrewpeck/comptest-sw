#include "ddd.hpp"
#include <stdexcept>
#include "sleep.hpp"
#include <cassert>

namespace DDD {
    static const uint32_t latch = 0x1 << 0;
    static const uint32_t mosi  = 0x1 << 1;
    static const uint32_t sclk  = 0x1 << 2;

    void setDelay (int ns_delay)
    {
        assert (ns_delay>=0);
        assert (ns_delay<=120);

        /* ddd chip steps in units of 2ns */
        int ddd_delay = ns_delay / 2;

        struct ddd_config dddconf;
        dddconf.ch1enable = 1;
        dddconf.ch2enable = 1;
        dddconf.ch3enable = 1;
        dddconf.ch4enable = 1;

        if (ddd_delay <= 15) {
            dddconf.ch1delay = ddd_delay;
            dddconf.ch2delay = 0;
            dddconf.ch3delay = 0;
            dddconf.ch4delay = 0;
        }
        else if (ddd_delay <= 30) {
            dddconf.ch1delay = 15;
            dddconf.ch2delay = ddd_delay-15;
            dddconf.ch3delay = 0;
            dddconf.ch4delay = 0;
        }
        else if (ddd_delay <= 45) {
            dddconf.ch1delay = 15;
            dddconf.ch2delay = 15;
            dddconf.ch3delay = ddd_delay-30;
            dddconf.ch4delay = 0;
        }
        else if (ddd_delay <= 60) {
            dddconf.ch1delay = 15;
            dddconf.ch2delay = 15;
            dddconf.ch3delay = 15;
            dddconf.ch4delay = ddd_delay-45;
        }

        setDelay (dddconf);
    }

    void setDelay (ddd_config config)
    {
        int adr = ADR_DDD;

        //printf("ddd1delay: %i\n", config.ch1delay);
        //printf("ddd2delay: %i\n", config.ch2delay);
        //printf("ddd3delay: %i\n", config.ch3delay);
        //printf("ddd4delay: %i\n", config.ch4delay);
        uint32_t data = 0;

        data |= (0x1 & config.ch4enable) << 19;
        data |= (0x1 & config.ch3enable) << 18;
        data |= (0x1 & config.ch2enable) << 17;
        data |= (0x1 & config.ch1enable) << 16;
        data |= (0xF & config.ch1delay ) << 12;
        data |= (0xF & config.ch2delay ) << 8;
        data |= (0xF & config.ch3delay ) << 4;
        data |= (0xF & config.ch4delay ) << 0;

        //printf("%05X\n", data);

        uint32_t status = Serial::read (adr);
        status &= ~sclk;    // CLK Low
        status &= ~mosi;    // Data Low
        status |=  latch;   // CS High
        Serial::write(adr, status);

        for (int iclk=0; iclk<20; iclk++) {
            status = Serial::read (adr);
            status &= ~sclk;    // CLK Low
            Serial::write(adr, status);

            status &= ~mosi;
            int databit =  (0x1 & (data >> (19-iclk))) ? mosi : 0; // Data
            //printf("\n%i",  0x1 & (data >> (19-iclk)));

            status |= databit;
            Serial::write(adr, status);

            status |= sclk;    // CLK High
            Serial::write(adr, status);
        }

        status &= ~sclk;    // CLK Low
        Serial::write(adr, status);

        status &= ~latch;   // Latch Low
        Serial::write(adr, status);

        // need at least 10 ns here
        usleep(1);

        status |=  latch;   // Latch High
        Serial::write(adr, status);
    }
}
