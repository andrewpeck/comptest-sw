#include "dac.hpp"
#include <cstdio>

void DAC::write (int dac_counts)
{
    /* Set Inactive */
    uint32_t status = serial.read(adr[m_dac]);
    status &= ~clk[m_dac];
    status &= ~din[m_dac];
    status |=  en[m_dac];
    serial.write(adr[m_dac], status);

    for (int iclk=0; iclk<14; iclk++)
    {
        /* Clock LOW */
        status &= ~clk[m_dac];
        serial.write(adr[m_dac], status);

        /* Chip select Active,
         * Set data bit */
        int data_bit = (0x1 & (dac_counts >> (13-iclk))) ? (din[m_dac]) : 0;
        status &= ~en[m_dac];
        status &= ~(din[m_dac]);
        status |= data_bit;
        serial.write(adr[m_dac], status);

        /* Clock High */
        status |= clk[m_dac];
        serial.write(adr[m_dac], status);
    }

    /* Set Inactive */
    status = serial.read(adr[m_dac]);
    status &= ~clk[m_dac]; // CLK Low
    status &= ~din[m_dac]; // Data Low
    status |=  en[m_dac];  // CS High
    serial.write(adr[m_dac], status);

}

float DAC::voltage(int dac_counts)
{
    float DAC_REFERENCE = 3.3f;
    return (dac_counts / static_cast<float>(0x3FF) * DAC_REFERENCE);
}

void DAC::writeVoltage (float voltage)
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

const uint32_t DAC::adr [] = {ADR_PULSE_CTRL, ADR_COMP_CONFIG};
const uint32_t DAC::en  [] = {0x1 << 5,       0x1 << 6};
const uint32_t DAC::din [] = {0x1 << 6,       0x1 << 7};
const uint32_t DAC::clk [] = {0x1 << 7,       0x1 << 8};

const float DAC::VREF = 3.3f;
