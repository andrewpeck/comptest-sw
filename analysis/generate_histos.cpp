#include <string>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>

#include "../src/test_params.hpp"

void generate_histos ()
{
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

    TCanvas *currents = new TCanvas("currents", "LCT Comparator Current Distributions", 1280,1024);
    currents->Divide(3,2);

    /* IBIAS */
    currents->cd(1);
    TH1D *h_ibias = new TH1D("ibias", "ibias current (uA)", 50, 0, 30);
    h_ibias->GetXaxis()->SetTitle("current (mA)");

    //TLeaf *leaf = results->GetLeaf("ibias");
    //for (int i=0; i<results->GetEntries(); i++) {
    //    //h_ibias->Fill(gRandom->Gaus(0,1));
    //    leaf->GetBranch()->GetEntry(i);
    //    h_ibias->Fill(leaf->GetValue());
    //}

    results->Draw("ibias>>ibias", "ibias>0");
    //h_ibias->Draw();
    currents->Update();

    low->DrawLine(REF_IBIAS_LOW,0,REF_IBIAS_LOW,gPad->GetUymax());
    high->DrawLine(REF_IBIAS_HIGH,0,REF_IBIAS_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    /* IAMP */
    currents->cd(2);

    TH1D *iamp = new TH1D("iamp", "iamp current (uA)", 50, 0, 20);
    iamp->GetXaxis()->SetTitle("current (mA)");
    results->Draw("iamp>>iamp", "iamp>0");
    currents->Update();

    low->DrawLine(REF_IAMP_LOW,0,REF_IAMP_LOW,gPad->GetUymax());
    high->DrawLine(REF_IAMP_HIGH,0,REF_IAMP_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    currents->cd(3);
    TH1D *ifamp = new TH1D("ifamp", "ifamp current (uA)", 50, 0, 20);
    ifamp->GetXaxis()->SetTitle("current (mA)");
    results->Draw("ifamp>>ifamp", "ifamp>0");
    currents->Update();

    low->DrawLine(REF_IFAMP_LOW,0,REF_IFAMP_LOW,gPad->GetUymax());
    high->DrawLine(REF_IFAMP_HIGH,0,REF_IFAMP_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    currents->cd(4);
    TH1D *ioff = new TH1D("ioff", "ioff current (uA)", 50, 0, 5);
    ioff->GetXaxis()->SetTitle("current (mA)");
    results->Draw("ioff>>ioff", "ioff>0");
    currents->Update();

    low->DrawLine(REF_IOFF_LOW,0,REF_IOFF_LOW,gPad->GetUymax());
    high->DrawLine(REF_IOFF_HIGH,0,REF_IOFF_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    /* Supply Currents */

    currents->cd(5);
    TH1D *i3v3 = new TH1D("i3v3", "i3v3 current (mA)", 50, 0, 10);
    i3v3->GetXaxis()->SetTitle("current (mA)");
    results->Draw("i3v3>>i3v3", "i3v3>0");
    currents->Update();

    low->DrawLine(REF_I3V3_LOW,0,REF_I3V3_LOW,gPad->GetUymax());
    high->DrawLine(REF_I3V3_HIGH,0,REF_I3V3_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    currents->cd(6);
    TH1D *i5v0 = new TH1D("i5v0", "i5v0 current (mA)", 50, 0, 10);
    i5v0->GetXaxis()->SetTitle("current (mA)");
    results->Draw("i5v0>>i5v0", "i5v0>0");
    currents->Update();

    low->DrawLine(REF_I5V0_LOW,0,REF_I5V0_LOW,gPad->GetUymax());
    high->DrawLine(REF_I5V0_HIGH,0,REF_I5V0_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    /* Thresholds */

    TCanvas *threshoff = new TCanvas("thresholds", "LCT Comparator Thresholds and Offsets Distributions", 1024,768);
    threshoff->Divide(2,2);


    threshoff->cd(1);
    TH1D *thresh_l = new TH1D("thresh_l", "threshold left>right", 100, 0, 25);
    thresh_l->GetXaxis()->SetTitle("Threshold (mV)");
    results->Draw("thresh_l>>thresh_l", "thresh_l>0");
    threshoff->Update();

    low->DrawLine(REF_THRESH_LOW,0,REF_THRESH_LOW,gPad->GetUymax());
    high->DrawLine(REF_THRESH_HIGH,0,REF_THRESH_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    threshoff->cd(2);
    TH1D *thresh_r = new TH1D("thresh_r", "threshold right>left", 100, 0, 25);
    thresh_r->GetXaxis()->SetTitle("Threshold (mV)");
    results->Draw("thresh_r>>thresh_r", "thresh_r>0");
    threshoff->Update();

    low->DrawLine(REF_THRESH_LOW,0,REF_THRESH_LOW,gPad->GetUymax());
    high->DrawLine(REF_THRESH_HIGH,0,REF_THRESH_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    /* Offsets */


    threshoff->cd(3);
    TH1D *offset_l = new TH1D("offset_l", "offset left>right", 100, 0, 2);
    offset_l->GetXaxis()->SetTitle("Offset (mV)");
    results->Draw("offset_l>>offset_l", "offset_l<100000");
    threshoff->Update();

    low->DrawLine(REF_OFFSET_LOW,0,REF_OFFSET_LOW,gPad->GetUymax());
    high->DrawLine(REF_OFFSET_HIGH,0,REF_OFFSET_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    threshoff->cd(4);
    TH1D *offset_r = new TH1D("offset_r", "offset right>left", 100, 0, 2);
    offset_r->GetXaxis()->SetTitle("Offset (mV)");
    results->Draw("offset_r>>offset_r", "offset_r<10000");
    threshoff->Update();

    low->DrawLine(REF_OFFSET_LOW,0,REF_OFFSET_LOW,gPad->GetUymax());
    high->DrawLine(REF_OFFSET_HIGH,0,REF_OFFSET_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    // /* Threshold Delta Spread */

    TCanvas *delta = new TCanvas("Threshold Delta Spread", "LCT Comparator Threshold Delta Spread", 600,400);
    TH1D *delta_thresh = new TH1D("delta_thresh", "delta_thresh (thresh max-min)", 50, 0, 10);
    delta_thresh->GetXaxis()->SetTitle("delta_thresh (mV)");
    results->Draw("delta_thresh>>delta_thresh", "delta_thresh>0");
    delta->Update();

    low->DrawLine(REF_THRESH_DELTA_LOW,0,REF_THRESH_DELTA_LOW,gPad->GetUymax());
    high->DrawLine(REF_THRESH_DELTA_HIGH,0,REF_THRESH_DELTA_HIGH,gPad->GetUymax());
    low->Draw("same");
    high->Draw("same");
    currents->Update();

    rootfile->Write("",TObject::kOverwrite);
    //rootfile->Close();
}
