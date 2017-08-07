
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
#include <TThread.h>
#include <TSystem.h>
#include <TApplication.h>


#define FITS 1
//#define DRAW 1
//#define DRAWFITS 1
//int argc, char* argv []
void fit_channel (TH2F* h2, TH1F* h1, std::string channel, TCanvas* c1, TFile* hfile);
void* fit_thread (void* ptr);

struct FitParams {
    TCanvas* c1;
    TH2F* h2;
    TH1F* h1;
    std::string channel;
    TFile* hfile;
};

void fit_raws (std::string name="def") {

    gStyle->SetPalette(1);


    //TFile* hfile = new TFile("tmp2.root","READ","TMB Queue Model");

    std::string logfile = std::string("./results/") + name + ".root";

    gSystem->CopyFile("tmp2.root", logfile.c_str(), true);

    TFile* hfile = new TFile(logfile.c_str(),"UPDATE","LCT Comparator Test Results");

    //------------------------------------------------------------------------------------------------------------------
    // Thresholds & Offsets
    //------------------------------------------------------------------------------------------------------------------

    TH2F* thresholds_l = (TH2F*) hfile -> Get ("thresholds_l");
    TH2F* thresholds_r = (TH2F*) hfile -> Get ("thresholds_r");
    TH2F* offsets_l    = (TH2F*) hfile -> Get ("offsets_l");
    TH2F* offsets_r    = (TH2F*) hfile -> Get ("offsets_r");

    thresholds_l -> SetDirectory(nullptr);
    thresholds_r -> SetDirectory(nullptr);
    offsets_l    -> SetDirectory(nullptr);
    offsets_r    -> SetDirectory(nullptr);

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

    th1_thresh_l -> SetDirectory(hfile -> GetDirectory(""));
    th1_thresh_r -> SetDirectory(hfile -> GetDirectory(""));

    //-OFFSETS-H1-------------------------------------------------------------------------------------------------------

    min=convertBin(test_offset, 1);
    max=convertBin(test_offset, 1024);

    TH1F* th1_offset_l = new TH1F ("th1_offset_l", "offset l>r",  max*4, 0, max);
    TH1F* th1_offset_r = new TH1F ("th1_offset_r", "offset r>l",  max*4, 0, max);

    th1_offset_l->GetXaxis() -> SetTitle("offset (mv)");
    th1_offset_l->GetYaxis() -> SetTitle("counts");
    th1_offset_r->GetXaxis() -> SetTitle("offset (mv)");
    th1_offset_r->GetYaxis() -> SetTitle("counts");

    th1_offset_l->SetStats(1);
    th1_offset_r->SetStats(1);

    th1_offset_l -> SetDirectory(hfile -> GetDirectory(""));
    th1_offset_r -> SetDirectory(hfile -> GetDirectory(""));

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

#ifdef DRAW
    TCanvas * c1 = new TCanvas ();
    c1->SetWindowSize(512*4,512*2);
    c1->Divide(4,2);
    c1->cd();
#endif

    int ipad=1;
    for (auto & channel : meas_vec) {
        TH2F* h2 = meas_map[channel];
        h2->SetOption("COLZ");
        h2->SetContour(100);
#ifdef DRAW
        c1->cd(ipad);
        h2->Draw();
        double ymin = mean[channel] - spread[channel];
        double ymax = mean[channel] + spread[channel];
        drawLimitHlines(gPad, ymin, ymax);
        c1->Update();
#endif


        ipad++;
    }

#ifdef DRAW
    c1->Modified();
    c1->Update();
    // draw now
    gSystem->ProcessEvents();
#endif

    float err_rate [1024];
    float bin [1024];

    float fit_thresh [16];
    float fit_span   [16];
    float fit_width  [16];

    TF1*     fits   [16];

    TMarker* marks  [16];

    std::vector<void*> params_v;

    ipad = 1;
    for (auto& channel : meas_vec) {

        TH2F* h2 = meas_map[channel];
        TH1F* h1 = meas_map_h1[channel];

        struct FitParams* params = new FitParams;
        params->h2      = h2;
        params->h1      = h1;
        params->channel = channel;
#ifdef DRAW
        params->c1      = c1;
#else
        params->c1      = 0;
#endif
        params->hfile   = hfile;

        params_v.push_back(params);
    }

    TThread *fit0 = new TThread ("l0", fit_thread, (void*) params_v[0]);
    TThread *fit1 = new TThread ("l1", fit_thread, (void*) params_v[1]);
    TThread *fit2 = new TThread ("l2", fit_thread, (void*) params_v[2]);
    TThread *fit3 = new TThread ("l3", fit_thread, (void*) params_v[3]);

    fit0->Run();
    fit1->Run();
    fit2->Run();
    fit3->Run();

    TThread::Ps();
    gSystem->ProcessEvents();

    fit0->Join();
    fit1->Join();
    fit2->Join();
    fit3->Join();


#ifdef DRAW
    c1->cd();
#endif

    //hfile->Write();
    hfile->Close();

}

void* fit_thread (void* ptr) {

    struct FitParams* params = (FitParams*) ptr;

    fit_channel (params->h2, params->h1, params->channel, params->c1, params->hfile);

    return 0;
}

void fit_channel (TH2F* h2, TH1F* h1, std::string channel, TCanvas* c1, TFile* hfile) {

    TThread::Lock();

    float err_rate [1024];
    float bin [1024];

    float fit_thresh [16];
    float fit_span   [16];
    float fit_width  [16];

    TF1*     fits   [16];

    TMarker* marks  [16];

#ifdef DRAWFITS
      TCanvas * cn = new TCanvas ();
      cn->SetWindowSize(4*320,4*320);
      cn->Divide(4,4);
#endif

    int iscan = (channel=="thresh_r" || channel=="thresh_l") ? test_thresh : test_offset;
    int iside = (channel=="thresh_r" || channel=="offset_r") ? 1 : 0;

    TThread::UnLock();

    for (int istrip=0; istrip<16; istrip++) {

        for (int ibin=0;ibin<1024;ibin++) {
            err_rate [ibin] = h2->GetBinContent(istrip+1, ibin+1);
            bin [ibin] = ibin;
        }

        //--------------------------------------------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------------------------------------------

        TGraph* gr = new TGraph(1024,bin,err_rate);

        char *name;
        char *title;

        asprintf(&name, "%s_%d",          channel.c_str(), istrip);
        asprintf(&title,"%s Channel %d",  channel.c_str(), istrip);

        gr->SetName(name);
        gr->SetTitle(title);

        TThread::Lock();

        TF1 *f1 = new TF1("fit","1.0/2*(1-TMath::Erf(1/[0]*(x-[1])))", 0, 1024);

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
        TThread::UnLock();

        float p0   = f1->GetParameter(0);
        float p1   = f1->GetParameter(1);
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
        //TThread::Lock();
        if (fail) {
            gr->GetFunction("fit")->SetLineColor(kRed);
            //printf("Failed to fit on %s strip %i\n", channel.c_str(), istrip);
        }
        else {
            gr->GetFunction("fit")->SetLineColor(kGreen+3);
        }
        //TThread::UnLock();
#endif

        if (fail) {
            fit_width  [istrip] = f1->GetParameter(0);
            fit_thresh [istrip] = f1->GetParameter(1);
            //std::cout << "fail xmax=" << fit_thresh[istrip] << std::endl;
        }
        else {
            fit_width  [istrip] = f1->GetParameter(0);
            fit_thresh [istrip] = f1->GetParameter(1);
        }



        //--------------------------------------------------------------------------------------------------------------


        double val = (fail) ? h1->GetXaxis()->GetXmax() - 1e-9 :
            convertBin(iscan, fit_thresh[istrip]);


        h1->Fill(val);

        TThread::Lock();
        h1->SetFillColor(kBlue-10);
#ifdef DRAW
        c1->cd (5+iscan*2+iside);
        h1->Draw();
        c1->Modified();
        c1->Update();

        double xmin = mean[channel] - spread[channel];
        double xmax = mean[channel] + spread[channel];

        drawLimitVlines(gPad, xmin, xmax);
#endif

        TThread::UnLock();

        TThread::Lock();
#ifdef DRAWFITS
        cn->cd(istrip+1);
        gr->Draw("SAME, *A");
        cn->Modified();
        cn->Update ();
        gStyle->SetOptFit();
        gPad->DrawFrame(0,0, 300, 1.1);
        gr-> SetMarkerStyle(3);
#endif
        hfile->cd();
        gr->Write();
        TThread::UnLock();

        TThread::Lock();

        TMarker * mark = new TMarker (0.5+double(istrip), val, 3);

        TAxis *yaxis = h2->GetYaxis();
        Double_t binCenter = yaxis->GetBinCenter(50);

        if (fail) {
            mark->SetY (-1 * binCenter);
            mark->SetMarkerColor(kRed);
            mark->SetMarkerSize(2);
        }

#ifdef DRAW
        c1->cd(1+iscan*2+iside);
        mark->Draw("SAME");
#endif
        TThread::UnLock();

    }

    TThread::Lock();
    h1->SetFillColor(kBlue-10);
    hfile->cd();
    h1->Write();

#ifdef DRAW
    c1->cd (5+iscan*2+iside);
    h1->Draw();
    c1->Modified();
    c1->Update();
    double xmin = mean[channel] - spread[channel];
    double xmax = mean[channel] + spread[channel];
    drawLimitVlines(gPad, xmin, xmax);
#endif


    TThread::UnLock();

}

int main (int argc, char *argv[]) {

    std::string file = "def";
    if (argc>1) {
        file = argv[1];
    }

    #ifdef DRAW
    TApplication theApp("App",&argc, argv);
    // your code
    // here you can Draw() things
    #endif

    fit_raws(file);

    #ifdef DRAW
    theApp.Run();
    #endif
    return 1;

}
