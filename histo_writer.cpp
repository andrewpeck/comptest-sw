#include <stdint.h>
#include <TH1F.h>
#include <TH2F.h>

#include <iostream>

#include "histo_writer.h"
#include "strip_sides.h"
#include "test_enums.h"

void histoWriter::fill1DHistogram (int scan, int channel, float* data_x, int n_entries) {

    int nbinsx = 1024;
    double xlow = 0;
    double xhigh = 0;

    char *name;
    char *title;

    asprintf(&name, "%s_%s", testname[scan], currents[channel]);
    asprintf(&title,"%s %s", testname[scan], currents[channel]);

    std::cout << "Generating histo for " << name << "    Title=" << title << std::endl;

    TH1F* h1 = new TH1F (name, title,  nbinsx, xlow, xhigh);


    h1 -> GetXaxis()->SetTitle(current_units[channel]);
    h1 -> GetYaxis()->SetTitle("counts");

    for (int i=0;i<n_entries;i++) {
        h1->Fill(data_x[i]);
    }

}

void histoWriter::fill2DHistogram (int scan, int strip, int side, float* data_x, float* data_y, float xmin, float xmax, float ymin, float ymax) {

    int nbinsx = 1024;
    double xlow = 0;
    double xhigh = 0;

    int nbinsy = 1000;
    double ylow = 0;
    double yhigh = 0;

    const char* side_str_short [] = {"l",   "r"  };
    const char* side_str_long  [] = {"l>r", "r>l"};

    char *name;
    char *title;

    asprintf(&name, "%s_%s%d", testname[scan],side_str_short[side], strip);
    asprintf(&title,"%s %s Channel %d", testname[scan],side_str_long[side], strip);

    std::cout << "Generating histo for " << name << "    Title=" << title << std::endl;

    TH2F* h2 = new TH2F (name, title,  nbinsx, xlow, xhigh, nbinsy, ylow, yhigh);

    for (int i=0;i<1024;i++) {
        h2->Fill(data_y[i], data_x[i]);
    }

    h2 -> GetXaxis()->SetTitle("mV");
    h2 -> GetYaxis()->SetTitle("error rate");

    h2 -> SetMarkerStyle (2);

    h2->Draw("COLZ");

}


void histoWriter::fillSummary (int scan, int strip, int side, float* data, int n_entries) {

    TH2F* h2;
    if (scan==test_thresh) {
        if (side==0)
            h2=thresholds_l;
        else
            h2=thresholds_r;
    }
    else {
        if (side==0)
            h2=offsets_l;
        else
            h2=offsets_r;
    }

    for (int ibin=0;ibin<1024;ibin++) {
        h2->Fill(strip, ibin, data[ibin]+0.0000000001); // add a tiny offset so that we don't draw as white
    }

    h2->Draw("COLZ");
}
