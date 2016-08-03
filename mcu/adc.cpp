#include "registers.hpp"
#include "adc.hpp"
#include <cassert>
#include <stdint.h>

namespace ADC {
    enum OutputFormat    { OF_BOB, OF_USB, OF_BTC };
    enum SamplePeriod    { SP_LONG, SP_SHORT };
    enum ReferenceSelect { RS_INTERNAL, RS_EXTERNAL };
    enum ConversionClock { CC_INTERNAL, CC_SCLK };
    enum InputMode       { IM_SINGLE_ENDED, IM_PSEUDO_DIFFERENTIAL };
    enum ConversionMode  { CM_ONE_SHOT, CM_REPEAT, CM_SWEEP, CM_REPEAT_SWEEP };
    enum SweepSequence   { SS_01234567, SS_02460246, SS_00224466, SS_02020202 };
    enum PinFunction     { PF_INT_BAR, PF_EOC };
    enum TriggerLevel    { TL_FULL, TL_75PC, TL_50PC, TL_25PC };

    struct   ADCconfig_t   {
        SamplePeriod     sp;
        ReferenceSelect  rs;
        ConversionClock  cc;
        ConversionMode   cm;
        SweepSequence    ss;
        InputMode        im;
        OutputFormat     of;
        PinFunction      pf;
        TriggerLevel     tl;
    };

    int  writeRead (uint16_t write_data);
    void write     (uint16_t write_data);

    void reset();
    void selectChannel(int channel);
    int readFIFO();
    void configure();
    uint32_t codeConfig(struct ADCconfig_t config);

    static const int INITIALIZE     = 0xA;

    //static const int CONFIG_DEFAULT = 0xF << 12;
    //static const int POWERDOWN      = 0x8 << 12;


    void init ()
    {
        reset();
        configure();
    }

    int read (int channel)
    {
        selectChannel(channel);
        int data = readFIFO();
        return data;
    }

    double readVoltage(int channel)
    {
        double VREF = 5.0f;
        double voltage = VREF * (double)read(channel) / (0x3FFF);
        return voltage;
    }

    int writeRead (uint16_t write_data)
    {
        assert (write_data <= 0xFFFF);

        static const uint32_t CS        = 0x1 << 0;
        static const uint32_t MOSI      = 0x1 << 1;
        static const uint32_t SCLK      = 0x1 << 2;
        static const uint32_t MISO      = 0x1 << 3;

        uint8_t  adr       = ADR_ADC;
        uint16_t read_data = 0;
        uint32_t status    = 0;

        /* Active */
        status |= CS;   /* CS High */
        Serial::write(adr, status);

        status &= ~CS;   /* CS LOW */
        Serial::write(adr, status);

        for (int iclk=0; iclk<16; iclk++) {
            int wr_bit =  (0x1 & (write_data >> (15-iclk))) ? (MOSI) : 0;

            status &= ~SCLK;     /* CLK Low */
            status &= ~MOSI;     /* Reset Data */
            status |=  wr_bit;   /* Write Data Bit */
            Serial::write(adr, status);

            /* Clock HIGH */
            status |= SCLK;
            Serial::write(adr, status);

            /* CLK Low */
            status &= ~SCLK;
            Serial::write(adr, status);

            status = Serial::read(adr);
            int rd_bit = 0x1&((status & MISO)>0);
            read_data |= rd_bit << (15-iclk);
        }

        /* Inactive */
        status &= ~SCLK; /* CLK Low */
        status &= ~MOSI; /* Data Low */
        status |=  CS;   /* CS High */
        Serial::write(adr, status);

        read_data >>= 2;
        read_data  &= 0x3FFF;
        return read_data;
    }

    void reset()
    {
        writeRead(INITIALIZE<<12);
    }

    void selectChannel(int channel)
    {
        static const int REFLOW  = 0xC;
        static const int REFMID  = 0xB;
        static const int REFHIGH = 0xD;

        if (channel < 8)
            channel = channel & 0x7;
        if (channel==8)
            channel = REFLOW;
        if (channel==9)
            channel = REFMID;
        if (channel==10)
            channel = REFHIGH;

        writeRead(channel << 12);
    }

    int readFIFO()
    {
        static const int READFIFO = 0xE;
        int data = writeRead(READFIFO << 12);
        return data;
    }

    void configure()
    {
        struct ADCconfig_t config;
        config.sp = SP_SHORT;
        config.rs = RS_EXTERNAL;
        config.cc = CC_INTERNAL;
        config.cm = CM_ONE_SHOT;
        config.ss = SS_01234567;
        config.im = IM_SINGLE_ENDED;
        config.of = OF_BOB;
        config.pf = PF_EOC;
        config.tl = TL_50PC;

        writeRead(codeConfig(config));
    }

    uint32_t codeConfig(struct ADCconfig_t config)
    {
        uint16_t cfr = 0;

        if (config.rs == RS_EXTERNAL)            cfr |= 0x800;
        if (config.of == OF_BTC)                 cfr |= 0x400;
        if (config.sp == SP_SHORT)               cfr |= 0x200;
        if (config.cc == CC_SCLK)                cfr |= 0x100;
        if (config.im == IM_PSEUDO_DIFFERENTIAL) cfr |= 0x080;

        switch(config.cm) {
            case CM_ONE_SHOT:                   break;
            case CM_REPEAT:       cfr |= 0x020; break;
            case CM_SWEEP:        cfr |= 0x040; break;
            case CM_REPEAT_SWEEP: cfr |= 0x060; break;
        };

        switch(config.ss) {
            case SS_01234567:               break;
            case SS_02460246: cfr |= 0x008; break;
            case SS_00224466: cfr |= 0x010; break;
            case SS_02020202: cfr |= 0x018; break;
        };

        if (config.pf == PF_EOC)
            cfr |= 0x004;

        switch(config.tl)
        {
            case TL_FULL:               break;
            case TL_75PC: cfr |= 0x001; break;
            case TL_50PC: cfr |= 0x002; break;
            case TL_25PC: cfr |= 0x003; break;
        }
        return ((INITIALIZE&0xF)<<12) | (cfr&0xFFF);
    }
}
