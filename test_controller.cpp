#include "rx_parser.h"
#include "now.h"
#include "serial.h"
#include "colors.h"
#include "test_scanner.h"
#include "histo_writer.h"

#include <TFile.h>

#include <iterator>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>


int main () {

    float data [1024];
    float amplitude [1024];
    for (int i=0; i<1024; i++) {
        amplitude[i] = i;
    }

    Scanner <float> scanner(data);

    //std::string filename = now();         // returns current date+time as string
    std::string filename = "tmp.root";

    TFile* hfile = new TFile(filename.c_str(),"RECREATE","LCT Comparator Test Results");
    hfile->Write();
    histoWriter writer(hfile);

    int fd = open ("/dev/cu.usbmodem222", O_RDWR | O_NOCTTY | O_SYNC);

    scanner.setSerialFd (fd);

    tcflush(fd, TCIOFLUSH); // clear buffer

    for (int iside = 0; iside < 2; iside++) {
        for (int istrip = 0; istrip < 16; istrip ++) {
            scanner.scanOffset(istrip, iside);
            scanner.flushController();
            // convert test settings to amplitude
            // convert uint16_t data to float ??
             writer.fill2DHistogram(test_offset, istrip, iside, data, amplitude);
        }
    }

    for (int iside = 0; iside < 2; iside++) {
        for (int istrip = 0; istrip < 16; istrip ++) {
            scanner.scanThresh(istrip, iside);
            scanner.flushController();
            // convert test settings to amplitude
            // convert uint16_t data to float ??
             writer.fill2DHistogram(test_thresh, istrip, iside, data, amplitude);
        }
    }

    for (int ichannel = 0; ichannel < 6; ichannel++) {
        scanner.scanCurrent(ichannel);
        scanner.flushController();
        writer.fill1DHistogram(test_currents, ichannel, data);
    }

    // for(auto& iter : params) {
    //     std::cout << "data :: " << iter.first << " = " << iter.second << std::endl;
    // }

    return 1;
}

