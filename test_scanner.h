#ifndef TEST_SCANNER_H
#define TEST_SCANNER_H

#include "rx_parser.h"
#include "serial.h"
#include "dict.h"

#include <stdint.h>

template <class T>
class Scanner {
    public:

        Scanner (T *data) :
            parser (&params, data),
            serial (_rx_buf)
        {

        } ;

        int scanCurrent (int channel) ;
        int scanOffset (int strip, int side) ;
        int scanThresh (int strip, int side) ;
        int checkParams (int test);
        void flushController ();

        inline void setSerialFd (int fd) { serial.setFd(fd); };

    private:
        int scanGeneric (int param1, int param2, int scan) ;
        int  readController ();

        uint16_t  data_size;

        Parser <T> parser;
        Serial serial;

        char _rx_buf [5000];

        Dict params;
};

#include "test_scanner.inc"

#endif /* TEST_SCANNER_H */
