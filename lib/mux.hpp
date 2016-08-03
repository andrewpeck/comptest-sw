#ifndef MUX_HPP
#define MUX_HPP
#include "serial.hpp"
#include <stdint.h>

namespace Mux {
    enum PulseLevel_t {HIGH, MED, LOW, OFF};

    struct MuxConfig_t {
        PulseLevel_t in[16];
        PulseLevel_t next;
        PulseLevel_t prev;
    };

    void configStrip (int strip, int side, struct MuxConfig_t &config);
    void configAllChannelsOff(struct MuxConfig_t &config);

    void writeHalfstripConfig (int strip, int side);

    uint32_t configToHalfstripMap (MuxConfig_t config);
    uint32_t configToCompoutExpect (MuxConfig_t config);

    void writeConfig (struct MuxConfig_t config);
    void writeHalfstripsExpect (uint32_t halfstrips);
    void writeCompoutExpect (int compout_expect);

    int amp (PulseLevel_t level);
};

#endif
