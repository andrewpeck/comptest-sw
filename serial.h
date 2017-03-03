#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#include <termios.h> // Terminal IO

class Serial {

    public:

        Serial(char* rx_buf) : _rx_buf(rx_buf) {};

        int rx ();
        int tx (char *write_data, int write_size);
        void flush();
        void setFd (int fd);

    private:
        int fd_;

        char* _rx_buf;
};

#endif /* SERIAL_H */
