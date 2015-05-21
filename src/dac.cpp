#include "dac.hpp"
#include <cstdio>
#include <cassert>

void DAC::write (int dac_counts)
{
    assert (dac_counts < 2<<13);

    /* Set Inactive */
    //uint32_t status = Serial::read(adr[m_dac]);

    uint32_t status = 0x0009;  // inactive state
    status &= ~clk[m_dac];
    status &= ~din[m_dac];
    status |=  en[m_dac];
    Serial::write(adr[m_dac], status);

    for (int iclk=0; iclk<14; iclk++)
    {
        /* Clock LOW */
        status &= ~clk[m_dac];
        Serial::write(adr[m_dac], status);

        /* Chip select Active,
         * Set data bit */
        int data_bit = (0x1 & (dac_counts >> (13-iclk))) ? (din[m_dac]) : 0;
        status &= ~en[m_dac];
        status &= ~(din[m_dac]);
        status |= data_bit;
        Serial::write(adr[m_dac], status);

        /* Clock High */
        status |= clk[m_dac];
        Serial::write(adr[m_dac], status);
    }

    /* Set Inactive */
    status &= ~clk[m_dac]; // CLK Low
    status &= ~din[m_dac]; // Data Low
    status |=  en[m_dac];  // CS High
    Serial::write(adr[m_dac], status);

}

double DAC::voltage(int dac_counts)
{
    double DAC_REFERENCE = 3.3f;
    return (dac_counts / static_cast<double>(0x3FF) * DAC_REFERENCE);
}

void DAC::writeVoltage (double voltage)
{
    int dac_counts = static_cast<int>((0x3FF)*(voltage/VREF));
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

const uint32_t DAC::adr [] = {ADR_DAC,        ADR_DAC};
const uint32_t DAC::en  [] = {0x1 << 0,       0x1 << 3};
const uint32_t DAC::din [] = {0x1 << 1,       0x1 << 4};
const uint32_t DAC::clk [] = {0x1 << 2,       0x1 << 5};

const double DAC::VREF = 3.3f;
