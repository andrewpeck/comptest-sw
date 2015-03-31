#ifndef DDD_HPP
#define DDD_HPP

#include "serial.hpp"
#include "registers.hpp"
#include <stdint.h>

class DDD {
    public:
        struct ddd_config {
            bool ch1enable;
            int ch1delay;

            bool ch2enable;
            int ch2delay;

            bool ch3enable;
            int ch3delay;

            bool ch4enable;
            int ch4delay;
        };

        void setDelay (ddd_config config);
    private:
        static const uint32_t latch = 0x1 << 0;
        static const uint32_t mosi  = 0x1 << 1;
        static const uint32_t sclk  = 0x1 << 2;
        static const uint32_t miso  = 0x1 << 3;

        Serial serial;
};
#endif
