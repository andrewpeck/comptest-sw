#ifndef DAC_HPP
#define DAC_HPP

#include "serial.hpp"
#include "registers.hpp"
#include "dac.hpp"
#include <cstdio>

class DAC {
    public:
        void write (int dac_counts);
        void writeVoltage (double voltage);

        double voltage (int dac_counts);

        int m_dac;

    protected:
        static const double VREF;

        void setPulseDAC();
        void setCompDAC();
        static const int PULSE_DAC = 0;
        static const int COMP_DAC  = 1;


        static const uint32_t en [2];
        static const uint32_t din[2];
        static const uint32_t clk[2];
        static const uint32_t adr[2];
};

class PDAC : public DAC {
    public:
    PDAC ();
};

class CDAC : public DAC {
    public:
    CDAC ();
};


#endif
