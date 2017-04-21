#ifndef HISTO_WRITER_H
#define HISTO_WRITER_H

#include <TFile.h>
#include <TH2F.h>

#include "board_characteristics.h"

class histoWriter {

    public:
    histoWriter (TFile* hfile) :
        _hfile (hfile)

    {
                                //name            //title            //x  xmi xma//ybins  ymin               ymax                                 );
        thresholds_l = new TH2F ("thresholds_l" , "thresholds l>r" , 16 , 0 , 16 , 1024 , dac_start_thresh , dac_start_thresh+1024*dac_step_thresh);
        thresholds_r = new TH2F ("thresholds_r" , "thresholds r>l" , 16 , 0 , 16 , 1024 , dac_start_thresh , dac_start_thresh+1024*dac_step_thresh);

        offsets_r    = new TH2F ("offsets_r"    , "offsets r>l"    , 16 , 0 , 16 , 1024 , dac_start_offset , dac_start_offset+1024*dac_step_offset);
        offsets_l    = new TH2F ("offsets_l"    , "offsets l>r"    , 16 , 0 , 16 , 1024 , dac_start_offset , dac_start_offset+1024*dac_step_offset);

        h2_compout = new TH2F ("h2_compout"   , "compout response"    ,2 , 0 , 2 , 1024 , dac_start_thresh , dac_start_thresh+1024*dac_step_thresh);
        h2_timing  = new TH2F ("h2_timing"    , "pktime response", 8 , 0 , 8 , 25 , 0 , 25);
        h2_compin  = new TH2F ("h2_compin"    , "compin response",   2 , 0 , 2 , 1024 , dac_start_thresh , dac_start_thresh+1024*dac_step_thresh);
    }



    ~histoWriter () {
       _hfile->Write();
       _hfile->Close();
    }

    void fill1DHistogram (int scan, int channel, float* data_x, int n_entries=1024);
    void fill2DHistogram (int scan, int strip, int side, float* data_x, float* data_y, float xmin=0, float xmax=1000, float ymin=0, float ymax=15000);

    void fillSummary  (int scan, int strip, int side, float* data, int n_entries=1024);

    void fillTiming  (int pktime, int delta);

    TH2F* thresholds_l;
    TH2F* thresholds_r;
    TH2F* offsets_l;
    TH2F* offsets_r;
    TH2F* h2_timing;
    TH2F* h2_compin;
    TH2F* h2_compout;

    private:
    TFile * _hfile;
    TH2F* getH2 (int scan, int strip, int side);

};

#endif /* HISTO_WRITER_H */
