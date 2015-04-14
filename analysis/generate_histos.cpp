#include <string>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>

#include "../src/test_params.hpp"

struct PlotParams_t {
    const char* name;
    const char* title;
    int nbins;
    int xmin;
    int xmax;
    const char* xlabel;
    const char* data;
    const char* cuts;
    double ref_low;
    double ref_high;
};


void draw_plot (struct PlotParams_t params, TCanvas* canvas, TTree* tree, TLine* low, TLine* high)
{
    TH1D *histo = new TH1D(params.name, params.title, params.nbins, params.xmin, params.xmax);
    histo->GetXaxis()->SetTitle(params.xlabel);
    tree->Draw(params.data, params.cuts);
    canvas->Update();

    low->DrawLine(params.ref_low,0,params.ref_low,gPad->GetUymax());
    high->DrawLine(params.ref_high,0,params.ref_high,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    canvas->Update();
}

void generate_histos ()
{
    struct PlotParams_t ibias;
    ibias.name = "ibias";
    ibias.title = "ibias current (uA)";
    ibias.nbins = 50;
    ibias.xmin = 0;
    ibias.xmax = 30;
    ibias.xlabel = "current (uA)";
    ibias.data = "ibias>>ibias";
    ibias.cuts = "ibias > 0";
    ibias.ref_low = REF_IBIAS_LOW;
    ibias.ref_high = REF_IBIAS_HIGH;

    struct PlotParams_t iamp;
    iamp.name = "iamp";
    iamp.title = "iamp current (uA)";
    iamp.nbins = 50;
    iamp.xmin = 0;
    iamp.xmax = 20;
    iamp.xlabel = "current (uA)";
    iamp.data = "iamp>>iamp";
    iamp.cuts = "iamp > 0";
    iamp.ref_low = REF_IAMP_LOW;
    iamp.ref_high = REF_IAMP_HIGH;

    struct PlotParams_t ifamp;
    ifamp.name = "ifamp";
    ifamp.title = "ifamp current (uA)";
    ifamp.nbins = 50;
    ifamp.xmin = 0;
    ifamp.xmax = 20;
    ifamp.xlabel = "current (uA)";
    ifamp.data = "ifamp>>ifamp";
    ifamp.cuts = "ifamp > 0";
    ifamp.ref_low = REF_IFAMP_LOW;
    ifamp.ref_high = REF_IFAMP_HIGH;

    struct PlotParams_t ioff;
    ioff.name = "ioff";
    ioff.title = "ioff current (uA)";
    ioff.nbins = 50;
    ioff.xmin = 0;
    ioff.xmax = 5;
    ioff.xlabel = "current (uA)";
    ioff.data = "ioff>>ioff";
    ioff.cuts = "ioff > 0";
    ioff.ref_low = REF_IOFF_LOW;
    ioff.ref_high = REF_IOFF_HIGH;

    struct PlotParams_t i3v3;
    i3v3.name = "i3v3";
    i3v3.title = "i3v3 current (mA)";
    i3v3.nbins = 50;
    i3v3.xmin = 0;
    i3v3.xmax = 10;
    i3v3.xlabel = "current (mA)";
    i3v3.data = "i3v3>>i3v3";
    i3v3.cuts = "i3v3 > 0";
    i3v3.ref_low = REF_I3V3_LOW;
    i3v3.ref_high = REF_I3V3_HIGH;

    struct PlotParams_t i5v0;
    i5v0.name = "i5v0";
    i5v0.title = "i5v0 current (mA)";
    i5v0.nbins = 50;
    i5v0.xmin = 0;
    i5v0.xmax = 10;
    i5v0.xlabel = "current (mA)";
    i5v0.data = "i5v0>>i5v0";
    i5v0.cuts = "i5v0 > 0";
    i5v0.ref_low = REF_I5V0_LOW;
    i5v0.ref_high = REF_I5V0_HIGH;

    struct PlotParams_t thresh_l;
    thresh_l.name = "thresh_l";
    thresh_l.title = "threshold left>right";
    thresh_l.nbins = 100;
    thresh_l.xmin = 0;
    thresh_l.xmax = 25;
    thresh_l.xlabel = "Threshold (mV)";
    thresh_l.data = "thresh_l>>thresh_l";
    thresh_l.cuts = "thresh_l > 0";
    thresh_l.ref_low = REF_THRESH_LOW;
    thresh_l.ref_high = REF_THRESH_HIGH;

    struct PlotParams_t thresh_r;
    thresh_r.name = "thresh_r";
    thresh_r.title = "threshold right>left";
    thresh_r.nbins = 100;
    thresh_r.xmin = 0;
    thresh_r.xmax = 25;
    thresh_r.xlabel = "Threshold (mV)";
    thresh_r.data = "thresh_r>>thresh_r";
    thresh_r.cuts = "thresh_r > 0";
    thresh_r.ref_low = REF_THRESH_LOW;
    thresh_r.ref_high = REF_THRESH_HIGH;

    struct PlotParams_t offset_r;
    offset_r.name = "offset_r";
    offset_r.title = "offset left>right";
    offset_r.nbins = 100;
    offset_r.xmin = 0;
    offset_r.xmax = 2;
    offset_r.xlabel = "offset (mV)";
    offset_r.data = "offset_r>>offset_r";
    offset_r.cuts = "offset_r > 0";
    offset_r.ref_low = REF_OFFSET_LOW;
    offset_r.ref_high = REF_OFFSET_HIGH;

    struct PlotParams_t offset_l;
    offset_l.name = "offset_l";
    offset_l.title = "offset right>left";
    offset_l.nbins = 100;
    offset_l.xmin = 0;
    offset_l.xmax = 2;
    offset_l.xlabel = "offset (mV)";
    offset_l.data = "offset_l>>offset_l";
    offset_l.cuts = "offset_l > 0";
    offset_l.ref_low = REF_OFFSET_LOW;
    offset_l.ref_high = REF_OFFSET_HIGH;

    struct PlotParams_t delta_thresh;
    delta_thresh.name = "delta_thresh";
    delta_thresh.title = "delta_thresh (thresh max-min)";
    delta_thresh.nbins = 50;
    delta_thresh.xmin = 0;
    delta_thresh.xmax = 10;
    delta_thresh.xlabel = "delta_thresh (mV)";
    delta_thresh.data = "delta_thresh>>delta_thresh";
    delta_thresh.cuts = "delta_thresh > 0";
    delta_thresh.ref_low = REF_THRESH_DELTA_LOW;
    delta_thresh.ref_high = REF_THRESH_DELTA_HIGH;

    ////ibias->Fit("gaus");

    TFile *rootfile = new TFile("results.root", "UPDATE");
    TTree *results = (TTree *) rootfile->Get("Results");

    /* Bias Currents */

    TLine *low  = new TLine(REF_IBIAS_LOW,0,REF_IBIAS_LOW,0);
    TLine *high = new TLine(REF_IBIAS_HIGH,0,REF_IBIAS_HIGH,0);
    low->SetLineColorAlpha(kRed,0.15);
    low->SetLineStyle(2);
    high->SetLineStyle(2);
    high->SetLineColorAlpha(kRed,0.15);

    /* Currents */
    TCanvas *currents = new TCanvas("currents", "LCT Comparator Current Distributions", 1280,1024);
    currents->Divide(3,2);

    currents->cd(1);
    draw_plot(ibias, currents, results, low, high);
    currents->cd(2);
    draw_plot(iamp, currents, results, low, high);
    currents->cd(3);
    draw_plot(ifamp, currents, results, low, high);
    currents->cd(4);
    draw_plot(ioff, currents, results, low, high);
    currents->cd(5);
    draw_plot(i3v3, currents, results, low, high);
    currents->cd(6);
    draw_plot(i5v0, currents, results, low, high);

    /* Thresholds and Offsets */
    TCanvas *threshoff = new TCanvas("thresholds", "LCT Comparator Thresholds and Offsets Distributions", 1024,768);
    threshoff->Divide(2,2);

    threshoff->cd(1);
    draw_plot(thresh_l, threshoff, results, low, high);
    threshoff->cd(2);
    draw_plot(thresh_r, threshoff, results, low, high);
    threshoff->cd(3);
    draw_plot(offset_l, threshoff, results, low, high);
    threshoff->cd(4);
    draw_plot(offset_r, threshoff, results, low, high);

    /* Threshold Delta Spread */

    TCanvas *delta = new TCanvas("Threshold Delta Spread", "LCT Comparator Threshold Delta Spread", 600,400);
    draw_plot(delta_thresh, delta, results, low, high);
    rootfile->Write("",TObject::kOverwrite);
    //rootfile->Close();
}
