#include "dac.hpp"
#include <cstdio>

void DAC::write (int dac_counts)
{
    // Set Inactive
    uint32_t status = serial.read(adr[m_dac]);
    status &= ~clk[m_dac]; // CLK Low
    status &= ~din[m_dac]; // Data Low
    status |=  en[m_dac];  // CS High
    serial.write(adr[m_dac], status);

    for (int iclk=0; iclk<14; iclk++)
    {
        status &= ~clk[m_dac];
        serial.write(adr[m_dac], status);  // Clock LOW

        int data_bit = (0x1 & (dac_counts >> (13-iclk))) ? (din[m_dac]) : 0;
        status &= ~en[m_dac];              // Cs LOW
        status &= ~(din[m_dac]);           // Reset Data
        status |= data_bit;                // Write Data
        serial.write(adr[m_dac], status);

        status |= clk[m_dac];              // Clock HIGH
        serial.write(adr[m_dac], status);
    }

    // Return Inactive
    status = serial.read(adr[m_dac]);
    status &= ~clk[m_dac]; // CLK Low
    status &= ~din[m_dac]; // Data Low
    status |=  en[m_dac];  // CS High
    serial.write(adr[m_dac], status);

}
}

void DAC::writeVoltage (float voltage)
{
    int dac_counts = static_cast<int>((2<<14) * (voltage/VREF) - 1);
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

const uint32_t DAC::adr [] = {ADR_PULSE_CTRL, ADR_COMP_CONFIG};
const uint32_t DAC::en  [] = {0x1 << 5,       0x1 << 6};
const uint32_t DAC::din [] = {0x1 << 6,       0x1 << 7};
const uint32_t DAC::clk [] = {0x1 << 7,       0x1 << 8};

const float DAC::VREF = 3.3f;
