#ifndef EMULATOR_HPP
#define EMULATOR_HPP
#include <random>

class Emulator
{
    public:
        void write (uint8_t adr, uint32_t write_data);
        uint32_t read (uint8_t adr);
        Emulator();
        ~Emulator();
        static uint16_t cdac_value;
        static uint16_t pdac_value;

    private:

        std::default_random_engine generator;

        static uint32_t REG_COMP_CONFIG       ;
        static uint32_t REG_PULSE_CTRL        ;
        static uint32_t REG_MUX1              ;
        static uint32_t REG_MUX2              ;
        static uint32_t REG_HALFSTRIPS        ;
        static uint32_t REG_HALFSTRIPS_EXPECT ;
        static uint32_t REG_HALFSTRIPS_ERRCNT ;
        static uint32_t REG_THRESHOLDS_ERRCNT ;
        static uint32_t REG_COMPOUT_ERRCNT    ;
        static uint32_t REG_ADC               ;
        static uint32_t REG_DDD               ;

        int adc (int mosi, int sclk, int cs);
        void dac (int idac, int din, int sclk);
        void ddd (int mosi, int sclk, int latch);

        int adc_ibit;
        int cdac_ibit;
        int pdac_ibit;


        uint16_t cdac_word;
        uint16_t pdac_word;

        uint16_t adc_word;
};
#endif
