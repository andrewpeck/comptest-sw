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


void test_controller  (std::string modem= "/dev/cu.usbmodem401341") {

    float data_buf [1024];
    float amplitude [1024];
    for (int i=0; i<1024; i++) {
        amplitude[i] = i;
    }

    Scanner <float> scanner(data_buf);

    //std::string filename = now();         // returns current date+time as string
    std::string filename = "tmp.root";

    TFile* hfile = new TFile(filename.c_str(),"RECREATE","LCT Comparator Test Results");

    hfile->Write();

    histoWriter writer(hfile);

    int fd = open (modem.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    if (fd<0)
        return;

    scanner.setSerialFd (fd);

   //tcflush(fd, TCIOFLUSH); // clear buffer


    scanner.reset();
    sleep (2);
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
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

                 scanner.flushController();

                 convertCounts(data_buf, num_entries, num_pulses);

                 writer.fillSummary (iscan, istrip, iside, data_buf, num_entries);

     } } }

     scanner.flushController();

     //-----------------------------------------------------------------------------------------------------------------
     // Compin
     //-----------------------------------------------------------------------------------------------------------------

     scanner.init();
     scanner.setCompin(1);
     scanner.flushController();

     for (int iside = 0; iside < 2; iside++) {

         int istrip = 0;

         scanner.scanThresh(istrip, iside, dac_start_thresh, dac_step_thresh, num_pulses);

         scanner.flushController();

         convertCounts(data_buf, num_entries, num_pulses);

         writer.fillSummary (test_compin, istrip, iside, data_buf, num_entries);

     }
     scanner.flushController();

     //-----------------------------------------------------------------------------------------------------------------
     // Compout
     //-----------------------------------------------------------------------------------------------------------------

     scanner.init();

     scanner.flushController();

     for (int iside = 0; iside < 2; iside++) {

         int istrip = 15;

         scanner.scanCompout(istrip, iside, dac_start_thresh, dac_step_thresh, num_pulses);
    //   scanner.scanThresh(istrip, iside, dac_start_thresh, dac_step_thresh, num_pulses);

         scanner.flushController();

         convertCounts(data_buf, num_entries, num_pulses);

         writer.fillSummary (test_compout, istrip, iside, data_buf, num_entries);

     }

     scanner.flushController();

     //-----------------------------------------------------------------------------------------------------------------
     // Timing Scan
     //-----------------------------------------------------------------------------------------------------------------

     for (int istrip=0; istrip<15; istrip++) {
         for (int iside = 0; iside < 2; iside++) {
             std::vector<uint8_t>* deltas = scanner.scanTiming(1000, 5, istrip, iside);
             for (int pktime=0; pktime<8; pktime++) {
                 for(uint16_t time : deltas[pktime]) {
                     writer.fillTiming(pktime, time);
                 }
             }
     }}

     //-----------------------------------------------------------------------------------------------------------------
     // Currents
     //-----------------------------------------------------------------------------------------------------------------

    for (int ichannel = 0; ichannel < 6; ichannel++) {
        scanner.scanCurrent(ichannel);
        convertCurrents(data_buf, num_entries, ichannel);
        writer.fill1DHistogram(test_currents, ichannel, data_buf);
        scanner.flushController();
    }

 // for(auto& iter : params) {
 //     std::cout << "data :: " << iter.first << " = " << iter.second << std::endl;
 // }

    hfile->Write();
    close (fd);
}

int main (int argc, char *argv[]) {

    std::string modem = "/dev/cu.usbmodem401341";
    if (argc>1) {
        modem = argv[1];
    }

    test_controller(modem);

}
