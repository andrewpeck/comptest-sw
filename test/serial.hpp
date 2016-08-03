#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <ftdi.h>
#include <stdint.h>

namespace Serial {
    void open();
    void close();

    void write (uint8_t adr, uint32_t write_data);
    uint32_t read  (uint8_t adr);
};

#endif
