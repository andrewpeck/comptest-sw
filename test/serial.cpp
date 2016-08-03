#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "emulator.hpp"
#include "serial.hpp"
#include "registers.hpp"

// TODO: Software command to flush the serial
//       state machine (force it to return idle)
//       basically... need to send 4? bytes of data,
//       none of which should begin with 0x5 or 0xA

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
    const int pid = 0x6014;

    static int ft_write (uint8_t *write_data, int num_bytes);
    static int ft_read(uint8_t* read_data, int num_bytes);
    static struct ftdi_context *ftdi;


    static int f;
    static ftdi_interface interface = INTERFACE_ANY;

    void flushSerialStatemachine()
    {
        uint8_t wr_packet [8] = {0};
        ft_write (wr_packet,7);
    }

    void write (uint8_t adr, uint32_t write_data)
    {
        uint8_t cmd [1] = {static_cast<uint8_t>(0xFF & (WRITE_CMD | ((0xF & adr) << 4)))};
        ft_write (cmd, 1);


        uint8_t wr_packet [5];
        wr_packet[0] = 0xFF & (write_data >> 24);
        wr_packet[1] = 0xFF & (write_data >> 16);
        wr_packet[2] = 0xFF & (write_data >> 8);
        wr_packet[3] = 0xFF & (write_data >> 0);

        printf("serialwr: %02x %08x\n", *cmd, write_data);
        ft_write (wr_packet, 4);
    }

    uint32_t read  (uint8_t adr)
    {
        uint8_t cmd [1] = {static_cast<uint8_t>(0xFF & (READ_CMD | ((0xF & adr) << 4)))};
        ft_write (cmd, 1);

        printf("serialrd: %02x ", *cmd);

        // Read Returned data from USB FIFO
        uint8_t rd_packet [5] = {0};
        ft_read (rd_packet,4);
        //ft_read (rd_packet);
        //ft_read (rd_packet+1); /* warning: pointer arithmetic */
        //ft_read (rd_packet+2); /* warning: pointer arithmetic */
        //ft_read (rd_packet+3); /* warning: pointer arithmetic */


        uint32_t read_data = 0;
        read_data |= rd_packet[0];
        read_data |= rd_packet[1] << 8;
        read_data |= rd_packet[2] << 16;
        read_data |= rd_packet[3] << 24;
        //TODO : check the endianness of this

        printf("%08x\n", read_data);

        return read_data;
    }

    void open ()
    {

        // Init
        if ((ftdi = ftdi_new()) == 0)
        {
            fprintf(stderr, "ftdi_new failed\n");
        }

        ftdi->type = TYPE_232H;
        ftdi->type = TYPE_232H;
        ftdi->usb_write_timeout = 25;
        ftdi->usb_read_timeout = 25;

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

        ftdi_usb_purge_buffers(ftdi);

        ftdi_set_bitmode(ftdi,0xFF, BITMODE_RESET); // reset

        usleep(1000);

        ftdi_set_bitmode(ftdi,0xFF, BITMODE_SYNCFF);

        usleep(1000);

        ftdi_set_latency_timer (ftdi, 16);

        flushSerialStatemachine();
    }

    void close()
    {
        ftdi_usb_close(ftdi);
        ftdi_free(ftdi);
    }

    int ft_write (uint8_t *write_data, int num_bytes)
    {
        usleep(10);
        while (true) {
            f = ftdi_write_data(ftdi, write_data, num_bytes);
            if (f>0)
                return f;
            printf("here\n");
        }
        //if(f>0) {
        //    printf("ftwr: %X\n", write_data);
        //}

    }

    int ft_read(uint8_t* read_data, int num_bytes)
    {
        while (true) {
            f = ftdi_read_data(ftdi, read_data, num_bytes);
            //f = ftdi_read_data(ftdi, read_data, 1);
            if (f > 0) {
                //printf("ftrd: %X\n", (uint32_t*)&read_data);
                return (f);
            }
            printf("%i bytes read\n", f);
        }
    }
#endif
}
