#include "registers.h"
#include "dac.hpp"

void DAC::write (int dac_counts)
{
    uint32_t status = serial.read(adr[m_dac]);
    status &= ~clk[m_dac]; // CLK Low
    status &= ~din[m_dac]; // Data Low
    status |=  en[m_dac];  // CS High
    serial.write(adr[m_dac], status);

    for (int iclk=0; iclk<14; iclk++)
    {
        status &= ~clk[m_dac];
        serial.write(adr[m_dac], status);                             // Clock LOW

        status &= ~en[m_dac];                                         // CS Low
        status &= ~(din[m_dac]);                                      // Reset Data
        status |=  (dac_counts >> (14-iclk)) ? (din[m_dac]) : 0; // Data

        serial.write(adr[m_dac], status);                             // Write Data

        status |= clk[m_dac];
        serial.write(adr[m_dac], status);                             // Clock HIGH
    }
}

void DAC::writeVoltage (float voltage)
{
    int dac_counts = static_cast<int>(2<<14*(voltage / VREF)-1);
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

const uint32_t DAC::adr = {ADR_PULSE_CTRL, ADR_COMP_CONFIG};
const uint32_t DAC::en  = {0x1 << 5,       0x1 << 6};
const uint32_t DAC::din = {0x1 << 6,       0x1 << 7};
const uint32_t DAC::en  = {0x1 << 7,       0x1 << 8};

const float DAC::VREF = 3.3f;
