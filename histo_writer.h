#ifndef HISTO_WRITER_H
#define HISTO_WRITER_H

#include <TFile.h>
#include <TH2F.h>
#include <vector>

#include "board_characteristics.h"
#include "data.h"

class histoWriter {

    public:
    histoWriter (TFile* hfile) :
        _hfile (hfile)

    {

        float start_thresh = convertBin(test_thresh, 0);
        float end_thresh = convertBin(test_thresh, 1024);

        float end_offset = convertBin(test_offset, 1024);
        float start_offset = convertBin(test_offset, 0);

        float start_compout = convertBin(test_compout, 0);
        float end_compout   = convertBin(test_compout, 1024);

                                //name            //title            //x  xmi xma//ybins  ymin               ymax                                 );
        thresholds_l = new TH2F ("thresholds_l" , "thresholds l>r" , 16 , 0 , 16 , 1024 , start_thresh , end_thresh);
        thresholds_r = new TH2F ("thresholds_r" , "thresholds r>l" , 16 , 0 , 16 , 1024 , start_thresh , end_thresh);

        offsets_r    = new TH2F ("offsets_r"    , "offsets r>l"    , 16 , 0 , 16 , 1024 , start_offset , end_offset);
        offsets_l    = new TH2F ("offsets_l"    , "offsets l>r"    , 16 , 0 , 16 , 1024 , start_offset , end_offset);

        h2_compout = new TH2F ("h2_compout"   , "OS1617 response"    ,2 , 0 , 2 , 1024 , start_compout , end_compout);

        h2_timing = new TH2F ("h2_timing"    , "pktime response", 8 , 0 , 8 , 25 , 0 , 25);

        h2_mode   = new TH2F ("h2_mode"    , "peakmode scan", 4 , 0 , 4 , 8 , 0 , 8);

        h2_compin  = new TH2F ("h2_compin"    , "compin errors",   2 , 0 , 2 , 1024 , start_thresh , end_thresh);

        h2_compin    -> GetZaxis() -> SetRangeUser(0, 1); // ... set the range ...
        h2_compout   -> GetZaxis() -> SetRangeUser(0, 1); // ... set the range ...

        thresholds_l -> GetZaxis() -> SetRangeUser(0, 1); // ... set the range ...
        thresholds_r -> GetZaxis() -> SetRangeUser(0, 1); // ... set the range ...
        offsets_l    -> GetZaxis() -> SetRangeUser(0, 1); // ... set the range ...
        offsets_r    -> GetZaxis() -> SetRangeUser(0, 1); // ... set the range ...

        std::vector<TH2F*> channels = {thresholds_l, thresholds_r, offsets_l, offsets_r};

        for (auto &h2 : channels) {
            h2 -> SetStats(0);
            h2->GetXaxis()->SetNdivisions(-16);
            h2->GetXaxis()->SetBinLabel(1  , "0");
            h2->GetXaxis()->SetBinLabel(2  , "1");
            h2->GetXaxis()->SetBinLabel(3  , "2");
            h2->GetXaxis()->SetBinLabel(4  , "3");
            h2->GetXaxis()->SetBinLabel(5  , "4");
            h2->GetXaxis()->SetBinLabel(6  , "5");
            h2->GetXaxis()->SetBinLabel(7  , "6");
            h2->GetXaxis()->SetBinLabel(8  , "7");
            h2->GetXaxis()->SetBinLabel(9  , "8");
            h2->GetXaxis()->SetBinLabel(10 , "9");
            h2->GetXaxis()->SetBinLabel(11 , "A");
            h2->GetXaxis()->SetBinLabel(12 , "B");
            h2->GetXaxis()->SetBinLabel(13 , "C");
            h2->GetXaxis()->SetBinLabel(14 , "D");
            h2->GetXaxis()->SetBinLabel(15 , "E");
            h2->GetXaxis()->SetBinLabel(16 , "F");
        }

        //--------------------------------------------------------------------------------------------------------------

        h2_compin->GetXaxis()->SetBinLabel(1 , "left");
        h2_compin->GetXaxis()->SetBinLabel(2 , "right");
        h2_compin->GetXaxis()->SetTitle("Halfstrip");

        //--------------------------------------------------------------------------------------------------------------

        h2_compout->GetXaxis()->SetBinLabel(1 , "left");
        h2_compout->GetXaxis()->SetBinLabel(2 , "right");
        h2_compin->GetXaxis()->SetTitle("Halfstrip");

        //--------------------------------------------------------------------------------------------------------------

        h2_timing->GetXaxis()->SetBinLabel(1  , "0");
        h2_timing->GetXaxis()->SetBinLabel(2  , "1");
        h2_timing->GetXaxis()->SetBinLabel(3  , "2");
        h2_timing->GetXaxis()->SetBinLabel(4  , "3");
        h2_timing->GetXaxis()->SetBinLabel(5  , "4");
        h2_timing->GetXaxis()->SetBinLabel(6  , "5");
        h2_timing->GetXaxis()->SetBinLabel(7  , "6");
        h2_timing->GetXaxis()->SetBinLabel(8  , "7");

        h2_timing->GetXaxis()->SetTitle("Peak Time Setting");
        h2_timing->GetYaxis()->SetTitle("Triad Rx Delay (bx)");

        //--------------------------------------------------------------------------------------------------------------

        h2_mode->GetXaxis()->SetBinLabel(1 , "0");
        h2_mode->GetXaxis()->SetBinLabel(2 , "1");
        h2_mode->GetXaxis()->SetBinLabel(3 , "2");
        h2_mode->GetXaxis()->SetBinLabel(4 , "3");

        h2_mode->GetYaxis()->SetBinLabel(1 , "0");
        h2_mode->GetYaxis()->SetBinLabel(2 , "1");
        h2_mode->GetYaxis()->SetBinLabel(3 , "2");
        h2_mode->GetYaxis()->SetBinLabel(4 , "3");
        h2_mode->GetYaxis()->SetBinLabel(5 , "4");
        h2_mode->GetYaxis()->SetBinLabel(6 , "5");
        h2_mode->GetYaxis()->SetBinLabel(7 , "6");
        h2_mode->GetYaxis()->SetBinLabel(8 , "7");

        h2_mode->GetXaxis()->SetTitle("Peak Mode");
        h2_mode->GetYaxis()->SetTitle("Peak Time");
    }



    ~histoWriter () {



    }

    void fill1DHistogram (int scan, int channel, float* data_x, int n_entries=1024);
    void fill2DHistogram (int scan, int strip, int side, float* data_x, float* data_y, float xmin=0, float xmax=1000, float ymin=0, float ymax=15000);

    void fillSummary  (int scan, int strip, int side, float* data, int n_entries=1024);

    void fillTiming  (int pktime, int delta);
    void fillMode  (int pktime, int pkmode, int delta);

    TH2F* thresholds_l;
    TH2F* thresholds_r;
    TH2F* offsets_l;
    TH2F* offsets_r;
    TH2F* h2_timing;
    TH2F* h2_mode;
    TH2F* h2_compin;
    TH2F* h2_compout;

    private:
    TFile * _hfile;
    TH2F* getH2 (int scan, int strip, int side);

};

#endif /* HISTO_WRITER_H */
