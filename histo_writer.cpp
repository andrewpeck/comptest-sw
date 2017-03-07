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

}
