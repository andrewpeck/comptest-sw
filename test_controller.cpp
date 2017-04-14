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

int main (int argc, char *argv[]) {

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

    std::string ttyname = "/dev/cu.usbmodem401341";
    if (argc>1) {
        ttyname = argv[1];
    }
    int fd = open (ttyname.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

    scanner.setSerialFd (fd);

   //tcflush(fd, TCIOFLUSH); // clear buffer


    //scanner.reset();
    sleep (2);
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();
    scanner.flushController();

     scanner.flushController();
     tcflush(fd, TCIOFLUSH); // clear buffer

     //-----------------------------------------------------------------------------------------------------------------
     //
     //-----------------------------------------------------------------------------------------------------------------

     for (int iside = 0; iside < 2; iside++) {
         for (int istrip = 0; istrip < 16; istrip ++) {
             scanner.scanOffset(istrip, iside, dac_start_offset, dac_step_offset, num_pulses);
             scanner.flushController();

             for (int i=0; i<num_entries; i++) {
                 amplitude[i] = dac_start_offset + dac_step_offset*i;
             }

             convertCounts(data, num_entries, num_pulses);
//           convertAmplitudes(test_offset, dac_start, dac_step, amplitude, num_entries);

             //writer.fill2DHistogram(test_offset, istrip, iside, data, amplitude);
             writer.fillSummary (test_offset, istrip, iside, data);
         }
     }

     //------------------------------------------------------------------------------------------------------------------
     //
     //------------------------------------------------------------------------------------------------------------------

     scanner.flushController();
     for (int iside = 0; iside < 2; iside++) {
         for (int istrip = 0; istrip < 16; istrip ++) {
             scanner.scanThresh(istrip, iside, dac_start_thresh, dac_step_thresh, num_pulses);
             scanner.flushController();

             for (int i=0; i<num_entries; i++) {
                 amplitude[i] = dac_start_thresh + dac_step_thresh*i;
             }

             convertCounts(data, num_entries, num_pulses);
//           convertAmplitudes(test_thresh, dac_start_thresh, dac_step, amplitude, num_entries);

             //writer.fill2DHistogram(test_thresh, istrip, iside, data, amplitude);

             writer.fillSummary (test_thresh, istrip, iside, data);
         }
     }

     //-----------------------------------------------------------------------------------------------------------------
     //
     //-----------------------------------------------------------------------------------------------------------------

//  for (int ichannel = 0; ichannel < 6; ichannel++) {
//      scanner.scanCurrent(ichannel);
//      //convertCurrents(data, num_entries, ichannel);
//      writer.fill1DHistogram(test_currents, ichannel, data);
//      scanner.flushController();
//  }

 // for(auto& iter : params) {
 //     std::cout << "data :: " << iter.first << " = " << iter.second << std::endl;
 // }

    close (fd);

    return 1;
}
