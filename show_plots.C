
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
                  //int argc, char* argv []

void show_plots (std::string name="def") {

    gStyle->SetPalette(1);


    TFile* hfile = new TFile("tmp.root","READ","TMB Queue Model");


    std::string logfile = std::string("./results/") + name + ".root";

    TFile* outfile = new TFile(logfile.c_str(),"RECREATE","LCT Comparator Test Results");


    TCanvas * c1 = new TCanvas ();
    c1->SetWindowSize(512*4,512*2);
    c1->Divide(4,2);
    c1->cd();

    //------------------------------------------------------------------------------------------------------------------
    // Currents
    //------------------------------------------------------------------------------------------------------------------

    TCanvas * c_currents = new TCanvas ();
    c_currents->SetWindowSize(512*3,512*2);
    c_currents->Divide(3,2);

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

        c_currents->cd(i+1);

        currents_map[channel] ->Draw();

        TH1F* h1 = currents_map[channel];
        if ( h1->GetXaxis()->GetBinCenter(h1->FindFirstBinAbove(0,1))   < xmin ||
             h1->GetXaxis()->GetBinCenter(h1->FindLastBinAbove (0,1))   > xmax)
            currents_map[channel] ->SetFillColor(kRed);
        else
            currents_map[channel] ->SetFillColor(kGreen+1);

        currents_map[channel] ->SetDirectory(outfile->GetDirectory(""));

        c_currents->Update();


        drawLimitVlines ( gPad, xmin, xmax);

        i++;
    }

    c_currents->Update();
    gSystem->ProcessEvents();

    //------------------------------------------------------------------------------------------------------------------
    // Misc Plots
    //------------------------------------------------------------------------------------------------------------------

    TCanvas * c9 = new TCanvas ();
    c9->SetWindowSize(512*2, 512*2);
    c9->Divide(2,2);
    c9->cd();

    //-PEAK TIMING------------------------------------------------------------------------------------------------------

    TH2F* h2_timing = (TH2F*) hfile -> Get ("h2_timing");
    h2_timing->SetStats(0);
    c9->cd(1);
    h2_timing->SetOption("COLZ");
    h2_timing->Draw();
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
    c9->cd(2);
    h2_mode->SetOption("COLZ");
    h2_mode->Draw();
    h2_mode ->SetDirectory(outfile->GetDirectory(""));


    //-COMPIN-----------------------------------------------------------------------------------------------------------

    c9->cd (3);

    TH2F* compin = (TH2F*) hfile -> Get ("h2_compin");

    compin->SetStats(0);

    compin->SetOption("COLZ");
    compin ->SetDirectory(outfile->GetDirectory(""));
    compin->Draw();

    c9->Update();
    gSystem->ProcessEvents();

    //-COMPOUT----------------------------------------------------------------------------------------------------------

    c9->cd (4);

    TH2F* compout = (TH2F*) hfile -> Get ("h2_compout");

    compout->SetStats(0);
    compout ->SetDirectory(outfile->GetDirectory(""));

    compout->SetOption("COLZ");
    compout->Draw();
    compout->SetContour(100);

    c9->Update();
    gSystem->ProcessEvents();

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

    //-THRESHOLDS-H1----------------------------------------------------------------------------------------------------

    float max;
    float min;

    min=convertBin(test_thresh, 1);
    max=convertBin(test_thresh, 1024);

    TH1F* th1_thresh_l = new TH1F ("th1_thresh_l", "thresh l>r",  max*4, 0, max);
    TH1F* th1_thresh_r = new TH1F ("th1_thresh_r", "thresh r>l",  max*4, 0, max);

    th1_thresh_l->GetXaxis() -> SetTitle("threshold (mv)");
    th1_thresh_l->GetYaxis() -> SetTitle("counts");
    th1_thresh_r->GetXaxis() -> SetTitle("threshold (mv)");
    th1_thresh_r->GetYaxis() -> SetTitle("counts");

    th1_thresh_l->SetStats(1);
    th1_thresh_r->SetStats(1);

    //-OFFSETS-H1-------------------------------------------------------------------------------------------------------

    min=convertBin(test_offset, 1);
    max=convertBin(test_offset, 1024);

    TH1F* th1_offset_l = new TH1F ("th1_offset_l", "offset l>r",  max*4, 0, max);
    TH1F* th1_offset_r = new TH1F ("th1_offset_r", "offset r>l",  max*4, 0, max);

    th1_offset_l->GetXaxis() -> SetTitle("offset (mv)");
    th1_offset_l->GetYaxis() -> SetTitle("counts");
    th1_offset_r->GetXaxis() -> SetTitle("offset (mv)");
    th1_offset_r->GetYaxis() -> SetTitle("counts");

    th1_thresh_l->SetStats(1);
    th1_thresh_r->SetStats(1);

    //------------------------------------------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------------------------------------------


    std::map <std::string, TH2F*> meas_map;

    meas_map ["thresh_l"]  = thresholds_l;
    meas_map ["thresh_r"]  = thresholds_r;
    meas_map ["offset_l"]  = offsets_l;
    meas_map ["offset_r"]  = offsets_r;

    std::map <std::string, TH1F*> meas_map_h1;

    meas_map_h1 ["thresh_l"]  = th1_thresh_l;
    meas_map_h1 ["thresh_r"]  = th1_thresh_r;
    meas_map_h1 ["offset_l"]  = th1_offset_l;
    meas_map_h1 ["offset_r"]  = th1_offset_r;


    int ipad=1;
    for (auto & channel : meas_vec) {
        c1->cd(ipad);
        TH2F* h2 = meas_map[channel];
        h2 -> SetOption("COLZ");
        h2->Draw();
        h2->SetContour(100);
        c1->Update();

        double ymin = mean[channel] - spread[channel];
        double ymax = mean[channel] + spread[channel];
        drawLimitHlines(gPad, ymin, ymax);

        ipad++;
    }

    c1->Modified();
    c1->Update();

    // draw now
    gSystem->ProcessEvents();



    c1->cd(5);

#ifdef DRAWFITS
    TCanvas * c2 = new TCanvas ();
    c2->SetWindowSize(4*320,4*320);
    c2->Divide(4,4);

    TCanvas * c3 = new TCanvas ();
    c3->SetWindowSize(4*320,4*320);
    c3->Divide(4,4);

    TCanvas * c4 = new TCanvas ();
    c4->SetWindowSize(4*320,4*320);
    c4->Divide(4,4);

    TCanvas * c5 = new TCanvas ();
    c5->SetWindowSize(4*320,4*320);
    c5->Divide(4,4);
#endif


    float err_rate [1024];
    float bin [1024];

    float fit_thresh [16];
    float fit_span   [16];
    float fit_width  [16];

    TGraph*  graphs [16];
    TF1*     fits   [16];

    TMarker* marks  [16];


    ipad = 1;
    for (auto& channel : meas_vec) {

            TH2F* h2 = meas_map[channel];
            TH1F* h1 = meas_map_h1[channel];

            for (int istrip=0; istrip<16; istrip++) {

                for (int ibin=0;ibin<1024;ibin++) {
                    err_rate [ibin] = h2->GetBinContent(istrip+1, ibin+1);
                    bin [ibin] = ibin;
                }

                //--------------------------------------------------------------------------------------------------------------
                //
                //--------------------------------------------------------------------------------------------------------------

                TGraph* gr = new TGraph(1024,bin,err_rate);
                gStyle->SetOptFit();

                graphs[istrip] = gr;

                char *name;
                char *title;

                asprintf(&name, "%s_%d",          channel.c_str(), istrip);
                asprintf(&title,"%s Channel %d",  channel.c_str(), istrip);

                gr->SetName(name);
                gr->SetTitle(title);

                TF1 *f1 = new TF1("fit","1.0/2*(1-TMath::Erf(1/[0]*(x-[1])))", 0, 1024);

                //std::cout << "fitting on strip " << istrip << std::endl;

#ifdef FITS
                if (channel=="thresh_l" || channel=="thresh_r") {
                    f1->SetParLimits(0, 0.01,5);
                    f1->SetParLimits(1, 0,600);

                    f1->SetParameter(0, 0.01);
                    f1->SetParameter(1, 100);

                    gr->Fit("fit", "QMWC", "", 0, 512);
                }
                else if (channel=="offset_l" || channel=="offset_r") {

                    f1->SetParLimits(1, 0,1023);

                    if (channel=="offset_l") { // l>r
                        f1->SetParLimits(0, 0.01,40);
                        f1->SetParameter(0, 0.01     );
                        f1->SetParameter(1, 200 );
                        gr->Fit("fit", "QMWC", "", 0, 1024);
                    }
                    else if (channel=="offset_r") {
                        f1->SetParLimits(0, 0.01,10);
                        f1->SetParameter(0, 0.01  );
                        f1->SetParameter(1, 60 );
                        gr->Fit("fit", "QMWC", "", 0, 512);
                    }
                }
#endif

                float p0 = f1->GetParameter(0);
                float p1 = f1->GetParameter(1);
                float chi2 = f1->GetChisquare();

                // just check whether the fit failed
                bool fail=0;
                if (chi2 > 10) {
                    fail=true;
                }
                if (p0<0) {
                    fail=true;
                }
                if (p1>800) {
                    fail=true;
                }

#ifdef FITS
                if (fail) {
                    gr->GetFunction("fit")->SetLineColor(kRed);
                    printf("Failed to fit on %s strip %i\n", channel.c_str(), istrip);
                }
                else {
                    gr->GetFunction("fit")->SetLineColor(kGreen+3);
                }
#endif

                if (fail) {
                    fit_width  [istrip] = f1->GetParameter(0);
                    fit_thresh [istrip] = f1->GetParameter(1);
                    std::cout << "fail xmax=" << fit_thresh[istrip] << std::endl;

                }
                else {
                    fit_width  [istrip] = f1->GetParameter(0);
                    fit_thresh [istrip] = f1->GetParameter(1);
                }

                //--------------------------------------------------------------------------------------------------------------

                int iscan = (channel=="thresh_r" || channel=="thresh_l") ? test_thresh : test_offset;
                double val = (fail) ? h1->GetXaxis()->GetXmax() - 1e-9 : convertBin(iscan, fit_thresh[istrip]);

                h1->Fill(val);

#ifdef DRAWFITS
                if (channel=="offset_l") {
                    c2->cd(istrip+1);
                    gPad->DrawFrame(0,0, 300, 1.1);
                    gr->Draw("SAME, *A");
                    gr-> SetMarkerStyle(3);
                }
                else if (channel=="offset_r") {
                    c3->cd(istrip+1);
                    gPad->DrawFrame(0,0, 300, 1.1);
                    gr->Draw("SAME, *A");
                    gr-> SetMarkerStyle(3);
                }
                else if (channel=="thresh_l") {
                    c4->cd(istrip+1);
                    gPad->DrawFrame(0,0, 300, 1.1);
                    gr->Draw("SAME, *A");
                    gr-> SetMarkerStyle(3);
                }
                else if (channel=="thresh_r") {
                    c5->cd(istrip+1);
                    gPad->DrawFrame(0,0, 300, 1.1);
                    gr->Draw("SAME, *A");
                    gr-> SetMarkerStyle(3);
                }
                gSystem->ProcessEvents();
#endif

                c1->cd(ipad);

                TMarker * mark = new TMarker (0.5+double(istrip), val, 3);

                TAxis *yaxis = h2->GetYaxis();
                Double_t binCenter = yaxis->GetBinCenter(50);

                if (fail) {
                    mark->SetY (-1 * binCenter);
                    mark->SetMarkerColor(kRed);
                    mark->SetMarkerSize(2);
                }

                mark->Draw("SAME");

            }

            c1->cd (4+ipad);
            h1->SetFillColor(kBlue-10);
            h1->Draw();
            c1->Update();

            double xmin = mean[channel] - spread[channel];
            double xmax = mean[channel] + spread[channel];
            drawLimitVlines(gPad, xmin, xmax);

            gSystem->ProcessEvents();

            ipad++;
    }

    c1->cd();

    outfile->Write();
//    hfile->Write("",TObject::kOverwrite);

}
