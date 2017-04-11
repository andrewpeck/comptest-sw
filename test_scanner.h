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
            parser (&read_params, data),
            serial (_rx_buf)
        {

        } ;

        int scanCurrent (int channel) ;
        int scanOffset (int strip, int side, int dac_start, int dac_step, int num_pulses);
        int scanThresh (int strip, int side, int dac_start, int dac_step, int num_pulses);
        int checkParams (const Dict set_params);
        void flushController ();
        void reset ();

        inline void setSerialFd (int fd) { serial.setFd(fd); };

    private:
        int scanGeneric (Dict test_params);
        int  readController (int ireads);

        uint16_t  data_size;

        Parser <T> parser;
        Serial serial;

        char _rx_buf [5000];

        Dict read_params;
};

template <class T>
int Scanner<T>::scanCurrent (int channel) {

    Dict set_params;

    set_params ["CHANNEL"]       = channel;
    set_params ["STRIP"]       = channel;
    set_params ["TEST"]       = test_currents;

    return scanGeneric(set_params);
}

template <class T>
int Scanner<T>::scanGeneric (Dict test_params) {
    //printf("scanGeneric\n");


    int scan = test_params["TEST"];

    int strip      = test_params["STRIP"];
    int side       = test_params["SIDE"];
    int channel    = test_params["CHANNEL"];
    int dac_start  = test_params["DAC_START"];
    int dac_step   = test_params["DAC_STEP"];
    int num_pulses = test_params["NUM_PULSES"];

    char  tx_buf [128];


    if (scan==test_thresh || scan==test_offset) {

        printf("\nsys  :: %ss scan started on   strip=%02d side=%1d dac_start=%d dac_step=%d num_pulses = %d\n",
                testname_short[scan], strip, side, dac_start, dac_step, num_pulses);

        sprintf(tx_buf, "%s %i %i %i %i %i\r\n", testname_short[scan], strip, side, dac_start, dac_step, num_pulses);
    }
    else if (scan==test_currents) {
        printf("\nsys  :: %s scan started on   chann=%02d curr=%s\n", testname_short[scan], channel, currents[channel]);
        sprintf(tx_buf, "%s %i\r\n", testname_short[scan], test_params["CHANNEL"]);
    }

    printf("%s\n", tx_buf);
    serial.tx(tx_buf, sizeof(tx_buf)/sizeof(tx_buf[0]));

    // iread=1. read in parameters line
    // iread=2. read in data
    // iread=3. read in endline
    // iread=4. info
    // iread=5. info

    read_params.clear();

    int ireads = 3;
    readController(ireads);

    for(auto& iter : test_params) {
        std::cout << "set   :: " << iter.first << " = " << iter.second << std::endl;
    }

    for(auto& iter : read_params) {
        std::cout << "read   :: " << iter.first << " = " << iter.second << std::endl;
    }

    auto retval = checkParams (test_params);

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
int Scanner<T>::scanOffset (int strip, int side, int dac_start, int dac_step, int num_pulses) {
    //printf("scanOffset\n");

    Dict set_params;

    set_params ["TEST"]        = test_offset;
    set_params ["STRIP"]       = strip;
    set_params ["SIDE"]        = side;
    set_params ["DAC_START" ]  = dac_start;
    set_params ["DAC_STEP" ]   = dac_step;
    set_params ["NUM_PULSES" ] = num_pulses;

    return scanGeneric(set_params);
}

template <class T>
int Scanner<T>::scanThresh (int strip, int side, int dac_start, int dac_step, int num_pulses) {

    Dict set_params;

    set_params ["TEST"]        = test_thresh;
    set_params ["STRIP"]       = strip;
    set_params ["SIDE"]        = side;
    set_params ["DAC_START" ]  = dac_start;
    set_params ["DAC_STEP" ]   = dac_step;
    set_params ["NUM_PULSES" ] = num_pulses;

    return scanGeneric(set_params);
}

template <class T>
int Scanner<T>:: checkParams (Dict set_params)
{
    int retval=0;

    auto test = set_params["TEST"];

    if (read_params["TEST"] != test ) {
        printf("sys  :: ERROR -- correct test not seen\n");
        retval +=1;
    }

    if (read_params["START"] != 1) {
        printf("sys  :: ERROR -- start bit not seen\n");
        retval +=1;
    }

    if (read_params["END"] != 1) {
        printf("sys  :: ERROR -- end bit not seen\n");
        retval +=1;
    }

//    if (read_params["DATA_RX"] != 1) {
//        printf("sys  :: ERROR -- data not received\n");
//        retval +=1;
//    }

    if (test==test_offset || test==test_thresh) {

        if (read_params["STRIP"] != set_params["STRIP"]) {
            printf("sys  :: ERROR -- test readback on wrong strip\n");
            retval +=1;
        }
        if (read_params["SIDE"] != set_params["SIDE"]) {
            printf("sys  :: ERROR -- test readback on wrong side\n");
            retval +=1;
        }

        if (read_params["DAC_STEP"] != set_params["DAC_STEP"]) {
            printf("sys  :: ERROR -- readback wrong dac step size\n");
            retval +=1;
        }

        if (read_params["DAC_START"] != set_params["DAC_START"]) {
            printf("sys  :: ERROR -- readback wrong dac start count\n");
            retval +=1;
        }

        if (read_params["NUM_PULSES"] != set_params["NUM_PULSES"]) {
            printf("sys  :: ERROR -- readback wrong number of pulses\n");
            retval +=1;
        }
    }
    else if (test==test_currents) {
        if (read_params["CHANNEL"] != set_params["CHANNEL"]) {
            printf("sys  :: ERROR -- readback wrong channel\n");
            retval +=1;
        }
    }
    return retval;
}

template <class T>
int Scanner<T>::readController (int ireads)
{

    // debug printf("readController");
    int nth_read = 0;
    while (nth_read<ireads) {
        int n = serial.rx();

        if (n>0) {
            nth_read++;
            // debug
            printf("%s\n", _rx_buf);
            parser.parseBuffer(_rx_buf, n);
        }

    }

    return nth_read;
}

template <class T>
void Scanner<T>::flushController ()
{
        serial.flush();
}

template <class T>
void Scanner<T>::reset ()
{
    char msg [] = "reset\r\n";
    serial.tx(msg, 7);
}

#endif /* TEST_SCANNER_H */
