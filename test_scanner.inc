#include "colors.h"
#include "test_scanner.h"

template <class T>
int Scanner<T>::scanCurrent (int channel) {
    return scanGeneric (channel, 0, test_currents);
}

template <class T>
int Scanner<T>::scanGeneric (int param1, int param2, int scan) {

    int strip;
    int side;
    int channel;

    if (scan==0 || scan==1) {
        strip = param1;
        side  = param2;
        printf("\nsys  :: %ss scan started on   strip=%02d side=%1d\n", testname_short[scan], strip, side);
    }
    else if (scan==2) {
        channel = param1;
        printf("\nsys  :: %s scan started on   chann=%02d curr=%s\n", testname_short[scan], channel, currents[channel]);
    }

    char  tx_buf [80];
    sprintf(tx_buf, "%s %i %i", testname_short[scan], strip, side);
    serial.tx(tx_buf, sizeof(tx_buf)/sizeof(tx_buf[0]));

    params.clear();

    // 1. read in parameters line
    // 2. read in data
    // 3. read in endline
    // 4. info
    // 5. info

    int ireads = 5;
    for (int i=0; i<ireads; i++) {
    readController();
    }

    auto retval = checkParams (scan);

    if (scan==0 || scan==1) {
        if (retval==0) {
            printf("sys  :: %ss scan %s on strip=%02d side=%1d\n", testname_short[scan], COMPLETED, strip, side);
        }
        else {
            printf("sys  :: %ss scan %s on    strip=%02d side=%1d\n", testname_short[scan], FAILED, strip, side);
        }
    }

    else if (scan==2) {

        if (retval==0) {
            printf("sys  :: %s scan %s on chann=%02d curr=%s\n", testname_short[scan], COMPLETED, channel, currents[channel]);
        }
        else {
            printf("sys  :: %s scan %s on    chann=%02d curr=%s\n", testname_short[scan], FAILED, channel, currents[channel]);
        }
    }

    return retval;
}

template <class T>
int Scanner<T>::scanOffset (int strip, int side) {
    return scanGeneric(strip,side,(int) test_offset);
}

template <class T>
int Scanner<T>::scanThresh (int strip, int side) {
    return scanGeneric(strip,side,(int) test_thresh);
}

template <class T>
int Scanner<T>:: checkParams (int test)
{
    int retval=0;

    if (params["TEST"] != test_offset) {
        printf("sys  :: ERROR -- correct test not seen\n");
        retval +=1;
    }

    if (params["START"] != 1) {
        printf("sys  :: ERROR -- start bit not seen\n");
        retval +=1;
    }

    if (params["END"] != 1) {
        printf("sys  :: ERROR -- end bit not seen\n");
        retval +=1;
    }

    if (params["DATA_RX"] == 0) {
        printf("sys  :: ERROR -- data not received\n");
        retval +=1;
    }

    if (test==test_offset || test==test_thresh) {

        if (params["MAX"] == 0) {
            printf("sys  :: ERROR -- max amplitude not received\n");
            retval +=1;
        }

        if (params["STEP"] == 0) {
            printf("sys  :: ERROR -- step size not received\n");
            retval +=1;
        }

    }

    return retval;
}

template <class T>
int Scanner<T>::readController ()
{
    int n = serial.rx();

    if (n) {
        parser.parseBuffer(_rx_buf, n);
    }

    return n;
}

template <class T>
void Scanner<T>::flushController ()
{
    for (int i=0; i<1; i++) {
        readController();
    }
}
