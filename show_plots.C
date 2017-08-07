#include "autorange.h"
#include "drawLimitLines.h"
#include "limits.h"

#include "test_enums.h"
#include "board_characteristics.h"
#include "data.cpp"
#include <TH2F.h>
#include <TFile.h>
#include <TGraph.h>
#include <TF1.h>
#include <TMarker.h>
#include <TCanvas.h>
#include <TStyle.h>
#include "HistGetter.h"
#include <TSystem.h>

#define FITS 1
#define DRAWFITS 1
const bool DRAW = 0;
                  //int argc, char* argv []

void show_plots () {

    gStyle->SetPalette(1);

    TFile* hfile = new TFile("tmp.root","READ","TMB Queue Model");

    //std::string logfile = std::string("./results/") + name + ".root";

    TFile* outfile = new TFile("tmp2.root","RECREATE","LCT Comparator Test Results");
    #ifdef DRAW
    TCanvas * c1 = new TCanvas ();
    c1->SetWindowSize(512*2,512*2);
    c1->Divide(2,2);
    c1->cd();
    #endif

    //------------------------------------------------------------------------------------------------------------------
    // Currents
    //------------------------------------------------------------------------------------------------------------------

    #ifdef DRAW
    TCanvas * c_currents = new TCanvas ();
    c_currents->SetWindowSize(512*3,512*2);
    c_currents->Divide(3,2);
    #endif

    TH1F* iamp  = (TH1F*) hfile -> Get ("iamp") -> Clone();
    TH1F* ioff  = (TH1F*) hfile -> Get ("ioff") -> Clone();
    TH1F* ibias = (TH1F*) hfile -> Get ("ibias")-> Clone();
    TH1F* ifamp = (TH1F*) hfile -> Get ("ifamp")-> Clone();
    TH1F* i5v0  = (TH1F*) hfile -> Get ("i5v0") -> Clone();
    TH1F* i3v3  = (TH1F*) hfile -> Get ("i3v3") -> Clone();

    std::map <std::string, TH1F*> currents_map;

    currents_map ["iamp"]  = iamp;
    currents_map ["ioff"]  = ioff;
    currents_map ["ibias"] = ibias;
    currents_map ["ifamp"] = ifamp;
    currents_map ["i5v0"]  = i5v0;
    currents_map ["i3v3"]  = i3v3;

    int i=0;
    for(auto& channel : currents_vec) {

        double xmin = mean[channel] - spread[channel]*2;
        double xmax = mean[channel] + spread[channel]*2;

        autoRange (currents_map[channel] , xmin, xmax);

        xmin = mean[channel] - spread[channel];
        xmax = mean[channel] + spread[channel];

    #ifdef DRAW
        c_currents->cd(i+1);

        currents_map[channel] ->Draw();
     #endif

        TH1F* h1 = currents_map[channel];
        if ( h1->GetXaxis()->GetBinCenter(h1->FindFirstBinAbove(0,1))   < xmin ||
             h1->GetXaxis()->GetBinCenter(h1->FindLastBinAbove (0,1))   > xmax)
            currents_map[channel] ->SetFillColor(kRed);
        else
            currents_map[channel] ->SetFillColor(kGreen+1);

        currents_map[channel] ->SetDirectory(outfile->GetDirectory(""));

    #ifdef DRAW
        c_currents->Update();

        drawLimitVlines ( gPad, xmin, xmax);

#endif

        i++;
    }

    #ifdef DRAW
    c_currents->Update();
    #endif
    gSystem->ProcessEvents();

    //------------------------------------------------------------------------------------------------------------------
    // Misc Plots
    //------------------------------------------------------------------------------------------------------------------
    #ifdef DRAW
    TCanvas * c9 = new TCanvas ();
    c9->SetWindowSize(512*2, 512*2);
    c9->Divide(2,2);
    c9->cd();
#endif

    //-PEAK TIMING------------------------------------------------------------------------------------------------------

    TH2F* h2_timing = (TH2F*) hfile -> Get ("h2_timing");
    h2_timing->SetStats(0);
    #ifdef DRAW
    c9->cd(1);
    h2_timing->Draw();
    #endif
    h2_timing->SetOption("COLZ");
    h2_timing ->SetDirectory(outfile->GetDirectory(""));

    //-PEAK MODE--------------------------------------------------------------------------------------------------------

    TH2F* h2_mode = (TH2F*) hfile -> Get ("h2_mode");

    double modemin=1e9;
    for (int imode=0; imode<4; imode++) {
        for (int itime=0; itime<8; itime++) {
            auto val = h2_mode->GetBinContent(imode+1,  itime+1);
            if (val < modemin)
                modemin=val;
        }
    }

    modemin += 1e-9;

    for (int imode=0; imode<4; imode++) {
        for (int itime=0; itime<8; itime++) {
            h2_mode->SetBinContent (imode+1, itime+1, (h2_mode->GetBinContent(imode+1,  itime+1)-modemin)/32/5);
        }
    }

    h2_mode->SetContour(100);

    h2_mode->SetStats(0);
    #ifdef DRAW
    c9->cd(2);
    h2_mode->SetOption("COLZ");
    h2_mode->Draw();
    #endif
    h2_mode ->SetDirectory(outfile->GetDirectory(""));


    //-COMPIN-----------------------------------------------------------------------------------------------------------

    #ifdef DRAW
    c9->cd (3);
    #endif

    TH2F* compin = (TH2F*) hfile -> Get ("h2_compin");

    compin->SetStats(0);

    compin->SetOption("COLZ");
    compin ->SetDirectory(outfile->GetDirectory(""));
    #ifdef DRAW
    compin->Draw();

    c9->Update();
    #endif
    gSystem->ProcessEvents();

    //-COMPOUT----------------------------------------------------------------------------------------------------------

    #ifdef DRAW
    c9->cd (4);
    #endif

    TH2F* compout = (TH2F*) hfile -> Get ("h2_compout");

    compout->SetStats(0);
    compout ->SetDirectory(outfile->GetDirectory(""));

    compout->SetOption("COLZ");
    compout->SetContour(100);
    #ifdef DRAW
    compout->Draw();
    #endif

    #ifdef DRAW
    c9->Update();
    gSystem->ProcessEvents();
    #endif

    //------------------------------------------------------------------------------------------------------------------
    // Thresholds & Offsets
    //------------------------------------------------------------------------------------------------------------------

    TH2F* thresholds_l = (TH2F*) hfile -> Get ("thresholds_l");
    TH2F* thresholds_r = (TH2F*) hfile -> Get ("thresholds_r");
    TH2F* offsets_l    = (TH2F*) hfile -> Get ("offsets_l");
    TH2F* offsets_r    = (TH2F*) hfile -> Get ("offsets_r");

    thresholds_l -> SetDirectory(outfile -> GetDirectory(""));
    thresholds_r -> SetDirectory(outfile -> GetDirectory(""));
    offsets_l    -> SetDirectory(outfile -> GetDirectory(""));
    offsets_r    -> SetDirectory(outfile -> GetDirectory(""));

    hfile->Close();

    //------------------------------------------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------------------------------------------


    std::map <std::string, TH2F*> meas_map;

    meas_map ["thresh_l"]  = thresholds_l;
    meas_map ["thresh_r"]  = thresholds_r;
    meas_map ["offset_l"]  = offsets_l;
    meas_map ["offset_r"]  = offsets_r;


    int ipad=1;
    for (auto & channel : meas_vec) {

    #ifdef DRAW
        c1->cd(ipad);
    #endif

        TH2F* h2 = meas_map[channel];
        h2 -> SetOption("COLZ");
        h2->SetContour(100);
    #ifdef DRAW
        h2->Draw();
        c1->Update();
    #endif

        double ymin = mean[channel] - spread[channel];
        double ymax = mean[channel] + spread[channel];
    #ifdef DRAW
        drawLimitHlines(gPad, ymin, ymax);
    #endif

        ipad++;
    }

    #ifdef DRAW
    c1->Modified();
    c1->Update();
    #endif

    // draw now
    gSystem->ProcessEvents();

    #ifdef DRAW
    c1->cd(5);
    #endif

    outfile->Write();
}

int main () {
    show_plots();
}
