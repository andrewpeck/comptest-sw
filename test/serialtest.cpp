#include "serial.hpp"
#include <time.h>
#include <stdlib.h>

namespace Serial {
    int ft_write (uint8_t *write_data, int num_bytes);
    int ft_read(uint8_t* read_data, int num_bytes);
}

int main() {
    srand(time(NULL));
    Serial::open();
    uint8_t wr_byte [1];
    uint8_t rd_byte [1];
    for (int i=0; i<1000; i++)
    {
        uint32_t write_data = rand() & 0xFFFFFFFF;
        Serial::write(0, write_data);
        Serial::write(0, write_data);
        uint32_t read_data  = Serial::read(0);
        if (read_data!=write_data) {
            printf("write : %08X\n", write_data);
            printf("read  : %08X\n", read_data);
        }

        //wr_byte[0] = 0xAA;
        //Serial::ft_write(wr_byte,1);

        //printf("rd: 0x");
        //for (int i=0; i<4; i++) {
        //    Serial::ft_read(rd_byte,1);
        //    printf("%02X", *rd_byte);
        //}
        //printf("\n");



        // LOOPBACK wr_byte[0] = rand() % 32;
        // LOOPBACK Serial::ft_write(wr_byte,1);
        // LOOPBACK //printf("wr: 0x%02X\n", *wr_byte);
        // LOOPBACK Serial::ft_read(rd_byte,1);
        // LOOPBACK if (*wr_byte != *rd_byte)
        // LOOPBACK     printf("wr: 0x%02X rd: 0x%02X\n", *wr_byte, *rd_byte);
    }
    Serial::close();
}
