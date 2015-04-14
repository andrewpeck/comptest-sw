#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "serial.hpp"
#include "registers.hpp"

namespace Serial {
#ifdef emulate
    void write (uint8_t adr, uint32_t write_data) {
        Emulator::write(adr, write_data);
    }
    uint32_t read  (uint8_t adr) {
        return(Emulator::read(adr));
    }

    void open () {};
    void close () {};
#else

    const int vid = 0x0403;
    const int pid = 0x6010;
    const int baudrate = 115200;

    static int ft_write (uint8_t *write_data, int num_bytes);
    static int ft_read(uint8_t* read_data);
    static struct ftdi_context *ftdi;
    static int f = 0;
    static ftdi_interface interface = INTERFACE_ANY;
    void write (uint8_t adr, uint32_t write_data)
    {
        uint8_t cmd [1] = {static_cast<uint8_t>(0xFF & (WRITE_CMD | (0xF0 & adr << 4)))};
        ft_write (cmd, 1);


        uint8_t wr_packet [4];
        wr_packet[0] = 0xFF & (write_data >> 24);
        wr_packet[1] = 0xFF & (write_data >> 16);
        wr_packet[2] = 0xFF & (write_data >> 8);
        wr_packet[3] = 0xFF & (write_data >> 0);

        ft_write (wr_packet, 4);
    }

    uint32_t read  (uint8_t adr)
    {
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

        return read_data;
    }

    void open ()
    {
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
    }

    void close()
    {
        ftdi_usb_close(ftdi);
        ftdi_free(ftdi);
    }

    int ft_write (uint8_t *write_data, int num_bytes)
    {
        f = ftdi_write_data(ftdi, write_data, num_bytes);

        if(f>0) {
            fprintf(stderr, "wrote %d bytes\n", f);
        }

        return f;
    }

    int ft_read(uint8_t* read_data)
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
}
