#include <string>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>

void generate_histos ()
{
    TFile *rootfile = new TFile("results.root", "UPDATE");
    TTree *results = (TTree *) rootfile->Get("Results");

    //ibias->Fit("gaus");


    /* Bias Currents */

    TH1F *ibias = new TH1F("ibias", "ibias current (mA)", 100, 0, 40);
    ibias->GetXaxis()->SetTitle("current (mA)");
    results->Draw("ibias>>ibias", "ibias>0");
    ibias->Write("",TObject::kOverwrite);

    TH1F *iamp = new TH1F("iamp", "iamp current (mA)", 100, 0, 40);
    iamp->GetXaxis()->SetTitle("current (mA)");
    results->Draw("iamp>>iamp", "iamp>0");
    iamp->Write("",TObject::kOverwrite);

    TH1F *ifamp = new TH1F("ifamp", "ifamp current (mA)", 100, 0, 40);
    ifamp->GetXaxis()->SetTitle("current (mA)");
    results->Draw("ifamp>>ifamp", "ifamp>0");
    ifamp->Write("",TObject::kOverwrite);

    TH1F *ioff = new TH1F("ioff", "ioff current (mA)", 100, 0, 40);
    ioff->GetXaxis()->SetTitle("current (mA)");
    results->Draw("ioff>>ioff", "ioff>0");
    ioff->Write("",TObject::kOverwrite);

    /* Supply Currents */

    TH1F *i3v3 = new TH1F("i3v3", "i3v3 current (mA)", 100, 0, 40);
    i3v3->GetXaxis()->SetTitle("current (mA)");
    results->Draw("i3v3>>i3v3", "i3v3>0");
    i3v3->Write("",TObject::kOverwrite);

    TH1F *i5v0 = new TH1F("i5v0", "i5v0 current (mA)", 100, 0, 40);
    i5v0->GetXaxis()->SetTitle("current (mA)");
    results->Draw("i5v0>>i5v0", "i5v0>0");
    i5v0->Write("",TObject::kOverwrite);

    /* Thresholds */

    TH1F *thresh_l = new TH1F("thresh_l", "threshold left>right", 100, 0, 25);
    thresh_l->GetXaxis()->SetTitle("Threshold (mV)");
    results->Draw("thresh_l>>thresh_l", "thresh_l>0");
    thresh_l->Write("",TObject::kOverwrite);

    TH1F *thresh_r = new TH1F("thresh_r", "threshold right>left", 100, 0, 25);
    thresh_r->GetXaxis()->SetTitle("Threshold (mV)");
    results->Draw("thresh_r>>thresh_r", "thresh_r>0");
    thresh_r->Write("",TObject::kOverwrite);

    /* Offsets */

    TH1F *offset_l = new TH1F("offset_l", "offset left>right", 100, -10, 10);
    offset_l->GetXaxis()->SetTitle("Offset (mV)");
    results->Draw("offset_l>>offset_l", "offset_l<100000");
    offset_l->Write("",TObject::kOverwrite);

    TH1F *offset_r = new TH1F("offset_r", "offset right>left", 100, -10, 10);
    offset_r->GetXaxis()->SetTitle("Offset (mV)");
    results->Draw("offset_r>>offset_r", "offset_r<10000");
    offset_r->Write("",TObject::kOverwrite);


    /* Threshold Delta Spread */

    TH1F *delta_thresh = new TH1F("delta_thresh", "delta_thresh (thresh max-min)", 100, 0, 10);
    delta_thresh->GetXaxis()->SetTitle("delta_thresh (mV)");
    results->Draw("delta_thresh>>delta_thresh", "delta_thresh>0");
    delta_thresh->Write("",TObject::kOverwrite);

    rootfile->Close();
}
