#ifndef SERIAL_LIBFTDI_C
#define SERIAL_LIBFTDI_C

//#define ASYNC_WRITE
//#define ASYNC_READ
#include <libftdi1/ftdi.h>
//#include <ftdi.h>

    int ft_write (uint8_t *write_data, int num_bytes);
    int ft_read(uint8_t* read_data, int num_bytes);
    static struct ftdi_context *ftdi;

    void usleep2 (int usdelay)
    {
        static const int NANOS =  1000000000LL;
        static const int MICROS = 1000000LL;
        int nanodelay = usdelay * NANOS / MICROS;
        struct timespec delay;
        delay.tv_sec = 0;
        delay.tv_nsec = nanodelay;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &delay, NULL);
    }



    static int f;

    void flushSerialStatemachine()
    {
        uint8_t wr_packet [4] = {0,0,0,0};
        ft_write (wr_packet,4);
    }

    void write (uint8_t adr, uint32_t write_data)
    {
        uint8_t wr_packet [5];
        wr_packet[0] = {static_cast<uint8_t>(0xFF & (WRITE_CMD | ((0xF & adr) << 4)))};
        wr_packet[1] = 0xFF & (write_data >> 0);
        wr_packet[2] = 0xFF & (write_data >> 8);
        wr_packet[3] = 0xFF & (write_data >> 16);
        wr_packet[4] = 0xFF & (write_data >> 24);

        //printf("serialwr: %01x %08x\n", adr, write_data);
        ft_write (wr_packet, 5);
        //ft_write (wr_packet+0, 1);
        //ft_write (wr_packet+1, 1);
        //ft_write (wr_packet+2, 1);
        //ft_write (wr_packet+3, 1);
        //ft_write (wr_packet+4, 1);
    }

    uint32_t read  (uint8_t adr)
    {
        uint8_t cmd [1] = {static_cast<uint8_t>(0xFF & (READ_CMD | ((0xF & adr) << 4)))};
        ft_write (cmd, 1);

        //printf("serialrd: %02x ", *cmd);

        // Read Returned data from USB FIFO
        #ifdef ASYNC_READ
            #define FTOFFSET 0
        #else
            #define FTOFFSET 0
        #endif
        uint8_t rd_packet [4+FTOFFSET] = {0};
        ft_read (rd_packet,4+FTOFFSET);

        //ft_read (rd_packet);
        //ft_read (rd_packet+1); /* warning: pointer arithmetic */
        //ft_read (rd_packet+2); /* warning: pointer arithmetic */
        //ft_read (rd_packet+3); /* warning: pointer arithmetic */


        uint32_t read_data = 0;
        read_data |= rd_packet[0] << 0;
        read_data |= rd_packet[1] << 8;
        read_data |= rd_packet[2] << 16;
        read_data |= rd_packet[3] << 24;

        //printf("%08x\n", read_data);

        return read_data;
    }

    void open ()
    {
        // Init
        if ((ftdi = ftdi_new()) == 0)
        {
            fprintf(stderr, "ftdi_new failed\n");
            exit(-1);
        }

        ftdi->type = TYPE_232H;
        ftdi->type = TYPE_232H;
        //ftdi->usb_write_timeout = 10;
        //ftdi->usb_read_timeout  = 10;

        // Select interface
        ftdi_set_interface(ftdi, INTERFACE_ANY);

        // Open device
        f = ftdi_usb_open(ftdi, vid, pid);

        if (f < 0) {
            fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f,
                    ftdi_get_error_string(ftdi));
            exit(-1);
        }

        ftdi_usb_reset(ftdi); 

        ftdi_set_bitmode(ftdi,0, BITMODE_RESET); // reset
        ftdi_usb_purge_buffers(ftdi);
        //ftdi_set_baudrate(ftdi, 921600);

        ftdi_read_data_set_chunksize(ftdi, 6);
        ftdi_write_data_set_chunksize(ftdi, 6);

        usleep2(1000);

        ftdi_set_bitmode(ftdi,0,BITMODE_SYNCFF);

        usleep2(1000);

        ftdi_set_latency_timer (ftdi, 4);

        //flushSerialStatemachine();
        //flushSerialStatemachine();
    }

    void close()
    {
        ftdi_usb_purge_buffers(ftdi);
        ftdi_usb_close(ftdi);
        ftdi_free(ftdi);
    }

    int ft_write (uint8_t *write_data, int num_bytes)
    {
#ifdef ASYNC_WRITE
        auto res = ftdi_write_data_submit(ftdi, write_data, num_bytes);
        //usleep2(5);
        return (res->completed);
#else
        while (true) {
            f = ftdi_write_data(ftdi, write_data, num_bytes);
            if (f>0)
                return f;
            //printf("waiting to write...\n");
        }
#endif
    }

    int ft_read(uint8_t* read_data, int num_bytes)
    {
        #ifdef ASYNC_READ
            //usleep2(1); 
            auto res = ftdi_read_data_submit(ftdi, read_data, num_bytes);
            //return(ftdi_transfer_data_done(res)); 
            return(ftdi_transfer_data_done(res)); 
        #else
            while (true) {
                f = ftdi_read_data(ftdi, read_data, num_bytes);
                //f = ftdi_read_data(ftdi, read_data, 1);
                if (f > 0) {
                    //printf("ftrd: %X\n", (uint32_t*)&read_data);
                    return (f);
                }
                //printf("waiting to read...\n");
                //printf("%i bytes read\n", f);
            }
            #endif
    }

#endif
