#ifndef MUX_HPP
#define MUX_HPP
#include "serial.hpp"
#include <stdint.h>

/* int mux 0-17
 * int config 0, 1, 2, 3
 */

class Mux {
    public:
        void halfstripMaptoMux (uint32_t halfstrips);
        void pulseStripLH (int triad);
        void pulseStripRH (int triad);

    private:
        int setMuxConfig (int mux, int config);

        Serial serial;
};

#endif
