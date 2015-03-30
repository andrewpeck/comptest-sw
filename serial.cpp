#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ftdi.h>

void Serial::write (uint8_t adr, uint32_t write_data)
{
    uint8_t cmd = 0xFF & (WRITE_CMD | (0xF0 & adr << 4));
    ft_write (cmd, 1);


    uint8_t [4] wr_packet;
    wr_packet[0] = 0xFF & (write_data >> 24);
    wr_packet[1] = 0xFF & (write_data >> 16);
    wr_packet[2] = 0xFF & (write_data >> 8);
    wr_packet[3] = 0xFF & (write_data >> 0);

    ft_write (wr_packet, 4);
}

uint32_t Serial::read  (uint8_t adr)
{
    uint8_t cmd = 0xFF & (READ_CMD | (0xF0 & adr << 4));
    ft_write (cmd, 1);

    // Read Returned data from USB FIFO
    uint8_t [4] rd_packet;
    ft_read (rd_packet);
}

int Serial::Serial()
{
    // Init
    if ((ftdi = ftdi_new()) == 0)
    {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
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

int Serial::~Serial()
{
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
}

int Serial::ft_write (char *write_data, int num_bytes)
{
    f = ftdi_write_data(ftdi, write_data, num_bytes);

    if(f>0) {
        fprintf(stderr, "wrote %d bytes\n", f);
    }

    return f;
}

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
