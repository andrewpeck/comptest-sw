#include "rx_parser.h"
#include "now.h"
#include "test_enums.h"
#include "serial.h"
#include "colors.h"
#include "test_scanner.h"
#include "histo_writer.h"
#include "board_characteristics.h"

#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>

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

#include "data.cpp"


int test_controller  (std::string modem= "/dev/cu.usbmodem401341") {

    float data_buf [1024];
    float amplitude [1024];
    for (int i=0; i<1024; i++) {
        amplitude[i] = i;
    }

    Scanner <float> scanner(data_buf);

    //std::string filename = now();         // returns current date+time as string
    std::string filename = "tmp.root";

    TFile* hfile = new TFile(filename.c_str(),"RECREATE","LCT Comparator Test Results");

    //hfile->Write();

    histoWriter writer(hfile);

    int fd = open (modem.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    if (fd<0)
        return -1;

    scanner.setSerialFd (fd);

    scanner.reset();
    scanner.init();
    sleep (1);
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();

    tcflush(fd, TCIOFLUSH); // clear buffer

     //-----------------------------------------------------------------------------------------------------------------
     // Offsets + Thresholds
     //-----------------------------------------------------------------------------------------------------------------

     for (int iscan=0; iscan <2; iscan++) {
         for (int iside = 0; iside < 2; iside++) {
             for (int istrip = 0; istrip < 16; istrip ++) {

                 if (iscan==test_offset) scanner.scanOffset(istrip, iside, dac_start_offset, dac_step_offset, num_pulses);
                 else                    scanner.scanThresh(istrip, iside, dac_start_thresh, dac_step_thresh, num_pulses);

                 convertCounts(data_buf, num_entries, num_pulses);

                 writer.fillSummary (iscan, istrip, iside, data_buf, num_entries);

     } } }

     //-----------------------------------------------------------------------------------------------------------------
     // Compin
     //-----------------------------------------------------------------------------------------------------------------

     scanner.init();
     scanner.setCompin(1);

     for (int iside = 0; iside < 2; iside++) {

         int istrip = 0;

         scanner.scanThresh(istrip, iside, dac_start_thresh, dac_step_thresh, 1); // 1=num_pulses

         convertCounts(data_buf, num_entries, 1); // 1=num_pulses

         writer.fillSummary (test_compin, istrip, iside, data_buf, num_entries);

     }

     //-----------------------------------------------------------------------------------------------------------------
     // Compout
     //-----------------------------------------------------------------------------------------------------------------

     scanner.init();

     for (int iside = 0; iside < 2; iside++) {

         int istrip = 15;

         scanner.scanCompout(istrip, iside, dac_start_compout, dac_step_compout, 1); // 1=num_pulses

         convertCounts(data_buf, num_entries, num_pulses);

         writer.fillSummary (test_compout, istrip, iside, data_buf, num_entries);
     }

     //-----------------------------------------------------------------------------------------------------------------
     // Timing Scan
     //-----------------------------------------------------------------------------------------------------------------

     for (int istrip=0; istrip<15; istrip++) {
         for (int iside = 0; iside < 2; iside++) {
         for (int imode = 0; imode < 4; imode++) {
             std::vector<uint8_t>* deltas = scanner.scanTiming(1000, 5, istrip, iside, imode);
             for (int pktime=0; pktime<8; pktime++) {
                 for(uint16_t delta : deltas[pktime]) {

                     if (imode==2) writer.fillTiming(pktime, delta);

                     writer.fillMode(pktime, imode, delta);
                 }
             }
     }}}

     //-----------------------------------------------------------------------------------------------------------------
     // Currents
     //-----------------------------------------------------------------------------------------------------------------

    for (int ichannel = 0; ichannel < 6; ichannel++) {
        scanner.scanCurrent(ichannel);
        convertCurrents(data_buf, num_entries, ichannel);
        writer.fill1DHistogram(test_currents, ichannel, data_buf);
    }

    hfile->Write();
    close (fd);

    return 0;
}

int main (int argc, char *argv[]) {

    std::string modem = "/dev/cu.usbmodem401341";
    if (argc>1) {
        modem = argv[1];
    }

    return test_controller(modem);

}
