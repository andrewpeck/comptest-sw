#include "registers.hpp"
#include "adc.hpp"
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

    int spiWriteRead (uint16_t write_data);

    void initialize();
    void selectChannel(int channel);
    int readFIFO();
    void configure();
    uint32_t codeConfig(struct ADCconfig_t config);

    static const uint32_t cs        = 0x1 << 0;
    static const uint32_t mosi      = 0x1 << 1;
    static const uint32_t sclk      = 0x1 << 2;
    static const uint32_t miso      = 0x1 << 3;

    static const int READFIFO       = 0xE << 12;
    static const int CONFIG_DEFAULT = 0xF << 12;
    static const int REFLOW         = 0xC << 12;
    static const int REFMID         = 0xB << 12;
    static const int REFHIGH        = 0xD << 12;
    static const int POWERDOWN      = 0x8 << 12;
    static const int INITIALIZE     = 0xA << 12;

    struct ADCconfig_t {
        SamplePeriod sp;
        ReferenceSelect rs;
        ConversionClock cc;
        ConversionMode cm;
        SweepSequence ss;
        InputMode im;
        OutputFormat of;
        PinFunction pf;
        TriggerLevel tl;
    };

    void init ()
    {
        initialize();
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
        double voltage = VREF * (double)read(channel) / (0xFFF);
        return voltage;
    }

    int spiWriteRead (uint16_t write_data)
    {
        uint32_t adr = ADR_ADC;
        uint32_t status = Serial::read(adr);

        /* Inactive */
        status &= ~sclk; // CLK Low
        status &= ~mosi; // Data Low
        status &= ~cs;   // CS Low
        Serial::write(adr, status);
        uint32_t read_data = 0;

        int num_bits = 16;
        for (int iclk=0; iclk<num_bits; iclk++) {

            int data_bit =  (0x1 & (write_data >> (15-iclk))) ? (mosi) : 0;

            status &= ~sclk;     // CLK Low
            status &= ~mosi;     // Reset Data
            status |=  data_bit; // Write Data Bit
            Serial::write(adr, status);

            status |= sclk;
            Serial::write(adr, status); // Clock HIGH

            data_bit = !!(Serial::read(adr) & miso) << (15-iclk);

            read_data |= data_bit;
        }

        /* Inactive */
        status &= ~sclk; // CLK Low
        status &= ~mosi; // Data Low
        status |=  cs;   // CS High
        Serial::write(adr, status);

        read_data  = read_data >> 2;
        read_data &= 0x3FFF;
        return read_data;
    }

    void initialize()
    {
        spiWriteRead(INITIALIZE);
    }

    void selectChannel(int channel)
    {
        channel = channel & 0x7;
        spiWriteRead(channel << 12);
    }

    int readFIFO()
    {
        int data = spiWriteRead(READFIFO);
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

        spiWriteRead(codeConfig(config));
    }

    uint32_t codeConfig(struct ADCconfig_t config)
    {
        uint16_t cfr = 0;
        if (config.rs == RS_EXTERNAL)
            cfr |= 0x800;
        if (config.of == OF_BTC)
            cfr |= 0x400;
        if (config.sp == SP_SHORT)
            cfr |= 0x200;
        if (config.cc == CC_SCLK)
            cfr |= 0x100;
        if (config.im == IM_PSEUDO_DIFFERENTIAL)
            cfr |= 0x080;

        switch(config.cm) {
            case CM_ONE_SHOT:
                break;
            case CM_REPEAT:
                cfr |= 0x020;
                break;
            case CM_SWEEP:
                cfr |= 0x040;
                break;
            case CM_REPEAT_SWEEP:
                cfr |= 0x060;
                break;
        };

        switch(config.ss) {
            case SS_01234567:
                break;
            case SS_02460246:
                cfr |= 0x008;
                break;
            case SS_00224466:
                cfr |= 0x010;
                break;
            case SS_02020202:
                cfr |= 0x018;
                break;
        };

        if (config.pf == PF_EOC)
            cfr |= 0x004;

        switch(config.tl)
        {
            case TL_FULL:
                break;
            case TL_75PC:
                cfr |= 0x001;
                break;
            case TL_50PC:
                cfr |= 0x002;
                break;
            case TL_25PC:
                cfr |= 0x003;
                break;
        }
        return ((INITIALIZE&0xF)<<12) | (cfr&0xFFF);
    }

}
