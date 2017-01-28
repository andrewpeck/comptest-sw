#ifndef HISTO_WRITER_H
#define HISTO_WRITER_H 

#include <TFile.h>

class histoWriter {

    public:
    histoWriter (TFile* hfile) :
        _hfile (hfile)
    {}

    ~histoWriter () {
       _hfile->Write();
       _hfile->Close();
    }

    void fill1DHistogram (int scan, int channel, float* data_x, int n_entries=1024);
    void fill2DHistogram (int scan, int strip, int side, float* data_x, float* data_y, float xmin=0, float xmax=1000, float ymin=0, float ymax=15000);

    private:
    TFile * _hfile;

};

#endif /* HISTO_WRITER_H */
