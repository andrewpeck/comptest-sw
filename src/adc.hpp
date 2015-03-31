#ifndef ADC_HPP
#define ADC_HPP

#include <stdint.h>
#include "serial.hpp"

class ADC {
    public:
        int read (int channel);
        float readVoltage (int channel);
    private:
        enum OutputFormat    { OF_BOB, OF_USB, OF_BTC };
        enum SamplePeriod    { SP_LONG, SP_SHORT };
        enum ReferenceSelect { RS_INTERNAL, RS_EXTERNAL };
        enum ConversionClock { CC_INTERNAL, CC_SCLK };
        enum InputMode       { IM_SINGLE_ENDED, IM_PSEUDO_DIFFERENTIAL };
        enum ConversionMode  { CM_ONE_SHOT, CM_REPEAT, CM_SWEEP, CM_REPEAT_SWEEP };
        enum SweepSequence   { SS_01234567, SS_02460246, SS_00224466, SS_02020202 };
        enum PinFunction     { PF_INT_BAR, PF_EOC };
        enum TriggerLevel    { TL_FULL, TL_75PC, TL_50PC, TL_25PC };

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

        int spiWriteRead (uint16_t write_data);

        void initialize();
        void selectChannel(int channel);
        int readFIFO();
        void configure();
        uint32_t codeConfig(struct ADCconfig_t config);

        static const float VREF;

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


        Serial serial;
};

#endif
