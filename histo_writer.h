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
        thresholds_l = new TH2F ("thresholds_l" , "thresholds_l" , 16 , 0 , 16 , 1024 , dac_start_thresh , dac_start_thresh+1024*dac_step_thresh);
        thresholds_r = new TH2F ("thresholds_r" , "thresholds_r" , 16 , 0 , 16 , 1024 , dac_start_thresh , dac_start_thresh+1024*dac_step_thresh);

        offsets_r    = new TH2F ("offsets_r"    , "offsets_r"    , 16 , 0 , 16 , 1024 , dac_start_offset , dac_start_offset+1024*dac_step_offset);
        offsets_l    = new TH2F ("offsets_l"    , "offsets_l"    , 16 , 0 , 16 , 1024 , dac_start_offset , dac_start_offset+1024*dac_step_offset);

    }



    ~histoWriter () {
       _hfile->Write();
       _hfile->Close();
    }

    void fill1DHistogram (int scan, int channel, float* data_x, int n_entries=1024);
    void fill2DHistogram (int scan, int strip, int side, float* data_x, float* data_y, float xmin=0, float xmax=1000, float ymin=0, float ymax=15000);

    void fillSummary  (int scan, int strip, int side, float* data, int n_entries=1024);

    TH2F* thresholds_l;
    TH2F* thresholds_r;
    TH2F* offsets_l;
    TH2F* offsets_r;


    private:
    TFile * _hfile;

};

#endif /* HISTO_WRITER_H */
