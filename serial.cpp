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
    // memset(rx_buf, 0, sizeof(rx_buf));

    while(iframe < max_packet_length) {

        //--------------------------------------------------------------------------------------------------------------

        fd_set set;
        struct timeval timeout;
        int rv;
        int len = 100;
        int n = -1;

        FD_ZERO(&set); /* clear the set */
        FD_SET(fd_, &set); /* add our file descriptor to the set */

        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;

        rv = select(fd_ + 1, &set, NULL, NULL, &timeout);
        if (rv == -1)
            perror("select\n"); /* an error accured */
        else if(rv == 0) {
            if (debug) printf("sys  :: timeout\n"); /* a timeout occured */
        }
        else
            n = read(fd_, rx_char, sizeof(rx_char));

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
    return 0;
}

int Serial::tx (char *write_data, int write_size)
{
    // Write Command
    write (fd_, write_data, write_size-1);
    return EXIT_SUCCESS;
}

void Serial::setFd (int fd) {
    fd_ = fd;
}
