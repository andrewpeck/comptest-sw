#include "mux.hpp"
#include "registers.hpp"


//void halfstripMaptoMux (uint32_t halfstrips)
//{
//
//}

namespace Mux {
    int amp (PulseLevel_t level)
    {
        if (level==HIGH)
            return 3;
        else if (level==MED)
            return 2;
        else if (level==LOW)
            return 1;
        else
            return 0;
    }

    uint32_t configToHalfstripMap (MuxConfig_t config)
    {
        const uint8_t LEFT  = 0x1;
        const uint8_t RIGHT = 0x2;
        const uint8_t NONE  = 0x0;

        int L_gt_R[16] = {0};
        int C_gt_L[16] = {0};
        int C_gt_R[16] = {0};
        int R_gt_L[16] = {0};

        int strip = 0;
        /* First Strip */
        if (amp(config.prev) > amp(config.in[strip+1]))
            L_gt_R[strip] = 1;
        if (amp(config.in[strip]) > amp(config.in[strip+1]))
            C_gt_R[strip] = 1;
        if (amp(config.in[strip]) > amp(config.prev))
            C_gt_L[strip] = 1;
        if (amp(config.in[strip+1]) > amp(config.prev))
            R_gt_L[strip] = 1;

        /* Inner Strips */
        for (strip=1; strip<15; strip++) {
            if (amp(config.in[strip-1]) > amp(config.in[strip+1]))
                L_gt_R[strip] = 1;
            if (amp(config.in[strip]) > amp(config.in[strip+1]))
                C_gt_R[strip] = 1;
            if (amp(config.in[strip]) > amp(config.in[strip-1]))
                C_gt_L[strip] = 1;
            if (amp(config.in[strip+1]) > amp(config.in[strip-1]))
                R_gt_L[strip] = 1;
        }

        /* Last Strip */
        strip = 15;
        if (amp(config.in[strip-1]) > amp(config.next))
            L_gt_R[0] = 1;
        if (amp(config.in[strip]) > amp(config.next))
            C_gt_R[0] = 1;
        if (amp(config.in[strip]) > amp(config.in[strip-1]))
            C_gt_L[0] = 1;
        if (amp(config.next) > amp(config.in[strip-1]))
            R_gt_L[0] = 1;

        /* Assume everything is over threshold... since we don't really know
         * but follow the rest of the triad logic */

        int halfstrip[16] = {0};
        for (strip=0; strip<15; strip++) {
            if (L_gt_R[strip] && C_gt_L[strip])
                halfstrip[strip]=LEFT;
            else if (R_gt_L[strip] && C_gt_R[strip])
                halfstrip[strip]=RIGHT;
            else
                halfstrip[strip]=NONE;
        }

        uint32_t halfstrips=0;
        for (int i=0; i<16; i++) {
            halfstrips |= halfstrip[i] << i*2;
        }

        return halfstrips;
    }

    void writeHalfstripsExpect (uint32_t halfstrips)
    {
        uint8_t adr = ADR_MUX1;
        uint32_t status = halfstrips;
        Serial::write (adr, status);
    }

    void configStripLH (int strip, struct MuxConfig_t &config)
    {
        config.in[strip] = HIGH;
        if (strip>0)
            config.in[strip-1]=MED;
        else
            config.prev=MED;

        if (strip<15)
            config.in[strip+1]=LOW;
        else
            config.next=LOW;
    }

    uint32_t configToCompoutExpect (MuxConfig_t config)
    {
        //TODO: fill in this code
        return (1);
    }

    void configStripRH (int strip, struct MuxConfig_t &config)
    {
        config.in[strip] = HIGH;

        if (strip>0)
            config.in[strip-1]=LOW;
        else
            config.prev=LOW;

        if (strip<15)
            config.in[strip+1]=MED;
        else
            config.next=MED;
    }

    void configAllChannelsOff(struct MuxConfig_t &config)
    {
        config.next = OFF;
        config.prev = OFF;
        for (int i=0; i<16; i++)
            config.in[i] = OFF;
    }

    void writeConfig (struct MuxConfig_t config)
    {
        uint8_t adr = ADR_MUX1;
        uint32_t status = 0;
        status |= config.prev << 0;
        status |= config.next << 2;
        Serial::write(adr,status);

        adr = ADR_MUX2;
        status = 0;
        for (int i=0; i<16; i++) {
            status |= config.in[i] << i*2;
        }
        Serial::write(adr, status);
    }
}
