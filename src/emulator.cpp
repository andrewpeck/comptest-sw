#include "registers.hpp"
#include "emulator.hpp"
#include <ctime>

#include <random>
#include <stdexcept>

uint32_t Emulator::REG_COMP_CONFIG       = 0;
uint32_t Emulator::REG_PULSE_CTRL        = 0x1 << 21;  //pulser ready
uint32_t Emulator::REG_MUX1              = 0;
uint32_t Emulator::REG_MUX2              = 0;
uint32_t Emulator::REG_HALFSTRIPS        = 0;
uint32_t Emulator::REG_HALFSTRIPS_EXPECT = 0;
uint32_t Emulator::REG_HALFSTRIPS_ERRCNT = 1000;
uint32_t Emulator::REG_COMPOUT_ERRCNT    = 0;
uint32_t Emulator::REG_ADC               = 0;
uint32_t Emulator::REG_DDD               = 0;

uint16_t Emulator::cdac_value = 1234;
uint16_t Emulator::pdac_value = 1234;


Emulator::Emulator()
{
    generator.seed (time(NULL));
    adc_ibit  = 0;
    cdac_ibit = 0;
    pdac_ibit = 0;
}

Emulator::~Emulator() { }

uint32_t Emulator::read (uint8_t adr)
{
    switch (adr) {
        case ADR_COMP_CONFIG:
            return REG_COMP_CONFIG;
            break;
        case ADR_PULSE_CTRL:
            return REG_PULSE_CTRL;
            break;
        case ADR_MUX1:
            return REG_MUX1;
            break;
        case ADR_MUX2:
            return REG_MUX2;
            break;
        case ADR_HALFSTRIPS:
            return REG_HALFSTRIPS;
            break;
        case ADR_HALFSTRIPS_EXPECT:
            return REG_HALFSTRIPS_EXPECT;
            break;
        case ADR_HALFSTRIPS_ERRCNT:
            {
                std::normal_distribution<double> distribution(0.0,15.0);

                if ((distribution(generator)+pdac_value) > cdac_value)
                    REG_HALFSTRIPS_ERRCNT = 0;
                else
                    REG_HALFSTRIPS_ERRCNT = 1000 + (5-distribution(generator));
                return REG_HALFSTRIPS_ERRCNT;
                break;
            }
        case ADR_COMPOUT_ERRCNT:
            return REG_COMPOUT_ERRCNT;
            break;
        case ADR_ADC:
            {
                //printf("c. REG_ADC: %08X\n", REG_ADC);
                return REG_ADC;
                break;
            }
        case ADR_DDD:
            return REG_DDD;
            break;
        default:
            throw (std::runtime_error("Attempt to read non-existent register"));
            return 0xDEADBEEF;
            break;
    }
}

void Emulator::write (uint8_t adr, uint32_t write_data)
{
    switch (adr) {
        case ADR_COMP_CONFIG:
            {
                REG_COMP_CONFIG = write_data;

                //int pktime0 = 0x1 & (write_data >> 0);
                //int pktime1 = 0x1 & (write_data >> 1);
                //int pktime2 = 0x1 & (write_data >> 2);

                //int pkmode0 = 0x1 & (write_data >> 3);
                //int pkmode1 = 0x1 & (write_data >> 4);

                //int lctrst  = 0x1 & (write_data >> 5);

                int cdac_en   = 0x1 & (write_data >> 6);
                int cdac_din  = 0x1 & (write_data >> 7);
                int cdac_sclk = 0x1 & (write_data >> 8);

                if (!cdac_en && cdac_sclk) {
                    dac (0, cdac_din, cdac_sclk);
                }
                break;
            }

        case ADR_PULSE_CTRL:
            {
                REG_PULSE_CTRL = write_data;

                int pulsedac_en = 0x1 & (write_data >> 5);
                int pulsedac_din = 0x1 & (write_data >> 6);
                int pulsedac_sclk = 0x1 & (write_data >> 7);

                if (!pulsedac_en && pulsedac_sclk) {
                    dac (1, pulsedac_din, pulsedac_sclk);
                }

                break;
            }

        case ADR_MUX1:
            {
                REG_MUX1 = write_data;
                break;
            }

        case ADR_MUX2:
            {
                REG_MUX2 = write_data;
                break;
            }

        case ADR_ADC:
            {
                //printf("a. REG_ADC: %08X\n", REG_ADC);
                REG_ADC = write_data;

                int cs   = 0x1 & REG_ADC >> 0;
                int mosi = 0x1 & REG_ADC >> 1;
                int sclk = 0x1 & REG_ADC >> 2;

                if (sclk && !cs)
                {
                    int miso = adc(mosi, sclk, cs);
                    //printf("miso: %i\n", miso);
                    REG_ADC &= ~(0x1 << 3);
                    REG_ADC |=  (0x1 & miso) << 3;
                    //printf("b. REG_ADC: %08X\n", REG_ADC);
                }

                break;
            }


        case ADR_HALFSTRIPS_EXPECT:
            {
                REG_HALFSTRIPS_EXPECT = write_data;
            }

        case ADR_DDD:
            {
                REG_DDD = write_data;
                int latch = 0x1 & REG_DDD >> 0;
                int mosi  = 0x1 & REG_DDD >> 1;
                int sclk  = 0x1 & REG_DDD >> 2;
                ddd ( mosi, sclk, latch);
                break;
            }

        default:
            {
                throw (std::runtime_error("Attempt to write non-existent register"));
            }
    }
}

void Emulator::dac (int idac, int din, int sclk)
{
    if (idac==0) {
        cdac_word |= din << (13-cdac_ibit);
        cdac_ibit += 1;
        if (cdac_ibit==14) {
            cdac_value = cdac_word; // Update Comparator Value
            cdac_ibit = 0; // Reset
            cdac_word = 0; //Reset
            //printf("%i Comparator DAC Set to %i counts (%fV)\n", cdac_ibit, cdac_value, cdac_value * 3.3f / (0x3FF));
        }
    }

    if (idac==1) {
        //int data_bit = (0x1 & (dac_counts >> (13-iclk))) ? (din[m_dac]) : 0;      // Set Data, MSB first
        pdac_word |= din << (13-pdac_ibit);
        pdac_ibit += 1;
        if (pdac_ibit==14) {
            pdac_value = pdac_word;
            //printf("%i Pulse DAC Set to %i counts (%fV)\n", pdac_ibit, pdac_value, pdac_value * 3.3f / (0x3FF));
            pdac_ibit = 0;
            pdac_word = 0;
        }
    }

}

int Emulator::adc (int mosi, int sclk, int cs)
{
    static uint16_t word_out;
    //word_out = 0x55;

    adc_word |= mosi << (15-adc_ibit);

    if (adc_ibit==3) {
        int channel = (adc_word>>12) & 0xF;

        std::normal_distribution<double> distribution(0.0,20.0);
        double noise = distribution(generator);
        //printf("noise %f\n", noise);

        switch (channel) {
            case 0x0:
                word_out = 200+noise;
                break;
            case 0x1:
                word_out = 220+noise;
                break;
            case 0x2:
                word_out = 190+noise;
                break;
            case 0x3:
                word_out = 240+noise;
                break;
            case 0x4:
                word_out = 260+noise;
                break;
            case 0x5:
                word_out = 170+noise;
                break;
        }

        //printf("wordout: %i\n", word_out);

    }

    //printf("word_out: %1X\n", word_out);
    //int miso = word_out >> 0x1;
    int miso = 0x1 & ((word_out <<2) >> (15-adc_ibit));

    adc_ibit += 1;
    if (adc_ibit==16) {
        adc_word=0;
        adc_ibit=0;
    }

    //printf("miso: %i==", !!miso);
    return miso;
}

void Emulator::ddd (int mosi, int sclk, int latch)
{

}
