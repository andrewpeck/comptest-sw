#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <ftdi.h>
#include <stdint.h>
#include "emulator.hpp"

class Serial {
    public:
        Serial();
        ~Serial();

        void write (uint8_t adr, uint32_t write_data);
        uint32_t read  (uint8_t adr);

#ifdef emulate
        Emulator emu;
#else

    private:
        int ft_write (uint8_t *write_data, int num_bytes);
        int ft_read(uint8_t* read_data);

        struct ftdi_context *ftdi;
        int f = 0;
        const int vid = 0x0403;
        const int pid = 0x6010;

        const int baudrate = 115200;

        ftdi_interface interface = INTERFACE_ANY;
#endif
};

#endif
