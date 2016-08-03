#include "dac.hpp"
#include <cstdio>
#include <cassert>

void DAC::write (int dac_counts)
{
    assert (dac_counts <  (1<<14));
    assert (dac_counts >= 0);

    uint8_t adr = ADR_DAC;
    uint32_t status;
    status  =  0x0009;  // inactive state
    status &= ~en[m_dac]; // enable just this dac
    Serial::write(adr, status);

    for (int iclk=0; iclk<14; iclk++)
    {
        /* Chip select Active,
         * Set data bit */
        int data_bit = (0x1 & (dac_counts >> (13-iclk))) ? (din[m_dac]) : 0;
        status &= ~(din[m_dac]);
        status |= data_bit;
        Serial::write(adr, status);

        /* Clock High */
        status |= clk[m_dac];
        Serial::write(adr, status);

        /* Clock LOW */
        status &= ~clk[m_dac];
        Serial::write(adr, status);
    }

    /* Set Inactive */
    status = 0x0009;
    Serial::write(adr, status);

}

void DAC::writeThreshold (float millivolts)
{
    static const float VREF = 3602.0f;
    assert (millivolts>=0);
    int dac_counts = (VREF-millivolts)*((1<<14)-1)/VREF;
    write(dac_counts);
}

double DAC::voltage(int dac_counts)
{
    return (dac_counts / static_cast<double>(0x3FF) * VREF[m_dac]);
}

void DAC::writeVoltage (double voltage)
{
    int dac_counts = static_cast<int>((0x3FF)*(voltage/VREF[m_dac]));
    write(dac_counts);
}

void DAC::setCompDAC ()
{
    m_dac = COMP_DAC;
}

void DAC::setPulseDAC ()
{
    m_dac = PULSE_DAC;
}
CDAC::CDAC () {
    setCompDAC();
}
PDAC::PDAC () {
    setPulseDAC();
}
//                            pdac            cdac
const uint32_t DAC::en  [] = {0x1 << 0,       0x1 << 3};
const uint32_t DAC::din [] = {0x1 << 1,       0x1 << 4};
const uint32_t DAC::clk [] = {0x1 << 2,       0x1 << 5};

const double   DAC::VREF [] = {3.3f,5.0f};
