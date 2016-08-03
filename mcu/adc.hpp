#ifndef ADC_HPP
#define ADC_HPP

#include <stdint.h>
#include "serial.hpp"

namespace ADC {
    int    read        (int channel);
    double readVoltage (int channel);
    void   init        ();
}

#endif
