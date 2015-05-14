#ifndef SERIAL_D2XX_C
#define SERIAL_D2XX_C
#include "ftd2xx.h"

    FT_HANDLE ftHandle;
    uint8_t Mask = 0xff;
    uint8_t Mode;
    uint8_t LatencyTimer=16; //our default setting is 16
    FT_STATUS ftStatus = FT_Open(0, &ftHandle);


    int ft_write (uint8_t *write_data, int num_bytes) {
        DWORD dwBytesWritten;
        ftStatus = FT_Write(ftHandle, write_data, num_bytes, &dwBytesWritten);
        return dwBytesWritten;
    }

    int ft_read(uint8_t* read_data, int num_bytes)
    {
        DWORD dwBytesRead;
        ftStatus = FT_Read(ftHandle, read_data, num_bytes, &dwBytesRead);
        return dwBytesRead;
    }

    void write (uint8_t adr, uint32_t write_data)
    {
        uint8_t wr_packet [5];
        wr_packet[0] = {static_cast<uint8_t>(0xFF & (WRITE_CMD | ((0xF & adr) << 4)))};
        wr_packet[1] = 0xFF & (write_data >> 0);
        wr_packet[2] = 0xFF & (write_data >> 8);
        wr_packet[3] = 0xFF & (write_data >> 16);
        wr_packet[4] = 0xFF & (write_data >> 24);

        //printf("serialwr: %02x %08x\n", *cmd, write_data);
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
        uint8_t rd_packet [4] = {0};
        ft_read (rd_packet,4);
        //ft_read (rd_packet);
        //ft_read (rd_packet+1); /* warning: pointer arithmetic */
        //ft_read (rd_packet+2); /* warning: pointer arithmetic */
        //ft_read (rd_packet+3); /* warning: pointer arithmetic */


        uint32_t read_data = 0;
        read_data |= rd_packet[0] << 0;
        read_data |= rd_packet[1] << 8;
        read_data |= rd_packet[2] << 16;
        read_data |= rd_packet[3] << 24;
        //TODO : check the endianness of this

        //printf("%08x\n", read_data);

        return read_data;
    }

    void open () {

        FT_SetVIDPID(vid, pid);
        ftStatus = FT_Open(0, &ftHandle);
        if(ftStatus != FT_OK)
        {
            // FT_Open failed return;
            printf("Failed to open FTDI chip\n");
            return;
        }
        Mode = 0x00; //reset mode
        ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);
        usleep(10*1000);
        Mode = 0x40; //Sync FIFO mode
        ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);
        if (ftStatus == FT_OK)
        {
            ftStatus = FT_SetLatencyTimer (ftHandle, LatencyTimer);
            ftStatus = FT_SetUSBParameters(ftHandle, 0x10000 , 0x10000);
            ftStatus = FT_SetFlowControl(ftHandle,FT_FLOW_RTS_CTS,0 ,0);
            //access data from here
        }
        else
        {
            // FT_SetBitMode FAILED!
        }


    }


    void close ()
    {
        FT_Close(ftHandle);
    }

#endif
