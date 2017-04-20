#include "serial.h"
#include <cstdio>
#include <cstring>
#include <iostream>

#include <stdio.h>
#include <unistd.h>

bool debug = 0;

int Serial::rx ()
{
    int max_packet_length = 5000;
    char rx_char [1];
    int rx_buf_index = 0;
    int iframe = 0;
    memset(_rx_buf, 0, sizeof(_rx_buf[0]));

    int n = 0;
    while(iframe < max_packet_length) {

        //--------------------------------------------------------------------------------------------------------------

        fd_set set;
        struct timeval timeout;
        int len = 100;
        n=-1;

        FD_ZERO(&set); /* clear the set */
        FD_SET(fd_, &set); /* add our file descriptor to the set */

        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;

        int rv = select(fd_ + 1, &set, NULL, NULL, &timeout);
        if (rv == -1)
            perror("select\n"); /* an error accured */
        else if(rv == 0) {
            //printf("sys  :: timeout\n"); /* a timeout occured */
            return 0;
        }
        else

        n = read(fd_, rx_char, sizeof(rx_char));

        //printf("%c", rx_char[0]);
        //printf("%c", rx_char[0]);
        //if (rx_char[0]=='\r')
        //printf("cr");
        //if (rx_char[0]=='\n')
        //printf("lf");

        //-exit if no data was read ------------------------------------------------------------------------------------
        if (n<=0) break;

        // std::cout << rx_char[0] ;

         //std::cout << rx_char[0] << std::endl;
        _rx_buf [rx_buf_index] = rx_char[0];

        if (rx_char[0] == '\n' && _rx_buf[rx_buf_index-1] == '\r') {
            //std::cout << _rx_buf << std::endl;
            _rx_buf [rx_buf_index-1] = '\0';
            return (rx_buf_index-1);
        }
        rx_buf_index ++;

        //-increment frame counter--------------------------------------------------------------------------------------
        iframe++;
    }
    return n;
}

int Serial::tx (char *write_data, int write_size)
{
    // Write Command

    for (int i=0; i<write_size; i++) {

        //printf("%c\n", write_data[i]);
        write (fd_, write_data+i, 1);

        if (write_data[i] =='\n') {
            tcdrain(fd_);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

void Serial::setFd (int fd) {
    fd_ = fd;
}

void Serial::flush () {
   // rx();
    //tcflush(fd_, TCIOFLUSH); // clear buffer
    //printf("sys  :: flushed -- %s\n", _rx_buf);
}
