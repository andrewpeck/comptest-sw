#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <stdint.h>
#include "emulator.hpp"

namespace Serial {
    void open();
    void close();

    int      ft_write (uint8_t *write_data, int num_bytes);
    int      ft_read(uint8_t* read_data, int num_bytes);
    void     write (uint8_t adr, uint32_t write_data);
    uint32_t read  (uint8_t adr);
};

#endif
