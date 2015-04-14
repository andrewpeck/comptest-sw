#ifndef DDD_HPP
#define DDD_HPP

#include "serial.hpp"
#include "registers.hpp"
#include <stdint.h>

namespace DDD {
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

        void setDelay (int delay);
        void setDelay (ddd_config config);

};
#endif
