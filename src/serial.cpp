#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "serial.hpp"
#include "registers.hpp"

void Serial::write (uint8_t adr, uint32_t write_data)
{
#ifdef emulate
    emu.write(adr, write_data);
#else
    uint8_t cmd [1] = {static_cast<uint8_t>(0xFF & (WRITE_CMD | (0xF0 & adr << 4)))};
    ft_write (cmd, 1);


    uint8_t wr_packet [4];
    wr_packet[0] = 0xFF & (write_data >> 24);
    wr_packet[1] = 0xFF & (write_data >> 16);
    wr_packet[2] = 0xFF & (write_data >> 8);
    wr_packet[3] = 0xFF & (write_data >> 0);

    ft_write (wr_packet, 4);
#endif
}

uint32_t Serial::read  (uint8_t adr)
{
#ifdef emulate
    uint32_t read_data = emu.read(adr);
#else
    uint8_t cmd [1] = {static_cast<uint8_t>(0xFF & (READ_CMD | (0xF0 & adr << 4)))};
    ft_write (cmd, 1);

    // Read Returned data from USB FIFO
    uint8_t rd_packet [4];
    ft_read (rd_packet);

    uint32_t read_data = 0;
    read_data |= rd_packet[0];
    read_data |= rd_packet[1] << 8;
    read_data |= rd_packet[2] << 16;
    read_data |= rd_packet[3] << 24;
    //TODO : check the endianness of this
#endif

    return read_data;
}

Serial::Serial()
{
#ifdef emulate
#else
    // Init
    if ((ftdi = ftdi_new()) == 0)
    {
        fprintf(stderr, "ftdi_new failed\n");
    }

    // Select interface
    ftdi_set_interface(ftdi, interface);

    // Open device
    f = ftdi_usb_open(ftdi, vid, pid);

    if (f < 0)
    {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f,
                ftdi_get_error_string(ftdi));
        exit(-1);
    }
#endif
}

Serial::~Serial()
{
#ifdef emulate
#else
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
#endif
}

#ifdef emulate
#else
int Serial::ft_write (uint8_t *write_data, int num_bytes)
{
    f = ftdi_write_data(ftdi, write_data, num_bytes);

    if(f>0) {
        fprintf(stderr, "wrote %d bytes\n", f);
    }

    return f;
}
#endif

#ifdef emulate
#else
int Serial::ft_read(uint8_t* read_data)
{
    while (true) {
        f = ftdi_read_data(ftdi, read_data, 4);
        if (f > 0) {
            fprintf(stderr, "read %d bytes\n", f);
            return f;
        }
    }
}
#endif
