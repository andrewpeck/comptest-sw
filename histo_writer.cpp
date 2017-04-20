#include <stdint.h>
#include <TH1F.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TGraph.h>

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

    asprintf(&name, "%s", currents[channel]);
    asprintf(&title,"Current %s", currents[channel]);

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
    double xhigh = 1024;

    int nbinsy = 1000;
    double ylow = 0;
    double yhigh = 1;

    const char* side_str_short [] = {"l",   "r"  };
    const char* side_str_long  [] = {"l>r", "r>l"};

    char *name;
    char *title;

    asprintf(&name, "%s_%s%d", testname[scan],side_str_short[side], strip);
    asprintf(&title,"%s %s Channel %d", testname[scan],side_str_long[side], strip);

    std::cout << "Generating histo for " << name << "    Title=" << title << std::endl;

    //TH2F* h2 = new TH2F (name, title,  nbinsx, xlow, xhigh, nbinsy, ylow, yhigh);
    //TProfile* h2 = new TProfile (name, title,  nbinsx, xlow, xhigh, ylow, yhigh);

    //TCanvas *c1 = new TCanvas();

    TGraph* gr = new TGraph(1024,data_y,data_x);
    gr->SetName(name);
    gr->SetTitle(title);

    TF1 *f1 = new TF1("fit","[0]/2*(1-TMath::Erf([1]*(x-[2])))", 0, 1024);

    f1->SetParameter(0, 0.5);

    gr->Fit(f1, "", "", (scan==test_thresh) ? 100 : 40, 1000);

    gr->Write();
    gr->Draw();

    TF1 *f2 = new TF1("gauussed","2*[0]*[1] / TMath::Sqrt(TMath::Pi()) * (TMath::Exp(  -1*TMath::Power( [1] * (x-[2]) ,2))  )", 0, 1024);
    f2->SetNpx(1024);
    f1->SetNpx(1024);

    std::cout << f1->GetParameter(0) << std::endl;
    std::cout << f1->GetParameter(1) << std::endl;
    std::cout << f1->GetParameter(2) << std::endl;

    f2->SetParameter(0, f1->GetParameter(0));
    f2->SetParameter(1, f1->GetParameter(1));
    f2->SetParameter(2, f1->GetParameter(2));

    //f2->Write();
    delete f2;

    delete f1;
    delete gr;

//    for (int i=0;i<1024;i++) {
//        h2->Fill(data_y[i], data_x[i]);
//    }
//
//    h2 -> GetXaxis()->SetTitle("mV");
//    h2 -> GetYaxis()->SetTitle("error rate");
//
//    h2 -> SetMarkerStyle (2);
//
//    h2->Draw("COLZ");

}


void histoWriter::fillSummary (int scan, int strip, int side, float* data, int n_entries) {

    TH2F* h2;

    uint16_t start;
    uint16_t step;

    if (scan==test_thresh) {
        start=dac_start_thresh;
        step=dac_step_thresh;

        if (side==0)
            h2=thresholds_l;
        else
            h2=thresholds_r;
    }
    else {

        start=dac_start_offset;
        step=dac_step_offset;

        if (side==0)
            h2=offsets_l;
        else
            h2=offsets_r;
    }

    for (int ibin=0;ibin<1024;ibin++) {
        float dac = start + ibin*step;
        h2->Fill(strip, dac, data[ibin]+0.0000000001); // add a tiny offset so that we don't draw as white
//        h2->SetBinContent(strip+1, dac+1, data[ibin]+0.000000001);
    }

    h2->Draw("COLZ");

}

void histoWriter::fillTiming (int pktime, int delta) {

    TH2F* h2 = h2_timing;

    uint16_t start;
    uint16_t step;

    h2->Fill(pktime, delta); // add a tiny offset so that we don't draw as white

    h2->Draw("COLZ");
}
