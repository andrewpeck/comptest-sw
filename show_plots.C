
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
//#define DRAWFITS 1
                  //int argc, char* argv []
void show_plots (std::string name="def") {

    TFile* hfile = new TFile("tmp.root","READ","TMB Queue Model");

    TCanvas * c1 = new TCanvas ();
    c1->SetWindowSize(2400,1024);
    c1->Divide(6,2);
    c1->cd();

    //------------------------------------------------------------------------------------------------------------------
    // Currents
    //------------------------------------------------------------------------------------------------------------------

    TCanvas * currents = new TCanvas ();
    currents->SetWindowSize(1200,800);
    currents->Divide(3,2);


    TH1F* iamp  = (TH1F*) hfile -> Get ("iamp");
    TH1F* ioff  = (TH1F*) hfile -> Get ("ioff");
    TH1F* ibias = (TH1F*) hfile -> Get ("ibias");
    TH1F* ifamp = (TH1F*) hfile -> Get ("ifamp");
    TH1F* i5v0  = (TH1F*) hfile -> Get ("i5v0");
    TH1F* i3v3  = (TH1F*) hfile -> Get ("i3v3");

    currents->cd(1);
    iamp->SetFillColor(kRed);
    iamp->Draw();

    currents->cd(2);
    iamp->SetFillColor(kRed);
    ioff->Draw();

    currents->cd(3);
    iamp->SetFillColor(kRed);
    ibias->Draw();

    currents->cd(4);
    iamp->SetFillColor(kRed);
    ifamp->Draw();

    currents->cd(5);
    iamp->SetFillColor(kRed);
    i5v0->Draw();

    currents->cd(6);
    iamp->SetFillColor(kRed);
    i3v3->Draw();

    currents->Update();

    gSystem->ProcessEvents();

    //------------------------------------------------------------------------------------------------------------------
    // Peak Timing
    //------------------------------------------------------------------------------------------------------------------

    TH2F* h2_timing        = (TH2F*) hfile -> Get ("h2_timing");
    gStyle->SetOptStat(0);
    c1->cd(5);
    h2_timing->Draw("COLZ");

    //------------------------------------------------------------------------------------------------------------------
    // Compin
    //------------------------------------------------------------------------------------------------------------------

    c1->cd (11);

    TH2F* compin = (TH2F*) hfile -> Get ("h2_compin");
    compin->GetXaxis()->SetBinLabel(1 , "left");
    compin->GetXaxis()->SetBinLabel(2 , "right");

    //gStyle->SetOptStat(0);

    compin->GetZaxis()->SetRangeUser(0, 1); // ... set the range ...
    compin->Draw("COLZ");

    c1->Update();
    gSystem->ProcessEvents();

    //------------------------------------------------------------------------------------------------------------------
    // Compout
    //------------------------------------------------------------------------------------------------------------------

    c1->cd (12);

    TH2F* compout = (TH2F*) hfile -> Get ("h2_compout");
    compout->GetXaxis()->SetBinLabel(1 , "left");
    compout->GetXaxis()->SetBinLabel(2 , "right");

    gStyle->SetOptStat(0);

    compout->GetZaxis()->SetRangeUser(0, 1); // ... set the range ...
    compout->Draw("COLZ");
    compout->SetContour(100);

    c1->Update();
    gSystem->ProcessEvents();

    //------------------------------------------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------------------------------------------

    TH2F* thresholds_l_raw = (TH2F*) hfile -> Get ("thresholds_l");
    TH2F* thresholds_r_raw = (TH2F*) hfile -> Get ("thresholds_r");
    TH2F* offsets_l_raw    = (TH2F*) hfile -> Get ("offsets_l");
    TH2F* offsets_r_raw    = (TH2F*) hfile -> Get ("offsets_r");

    //hfile->Close();

//    std::string ttyname = "./results/def.root";
//    if (argc>1) {
//        ttyname = argv[1];
//    }
//
//    TFile* outfile = new TFile(filename.c_str(),"RECREATE","LCT Comparator Test Results");
//    hfile->Write();



    //-THRESHOLDS-------------------------------------------------------------------------------------------------------

    float max;
    float min;

    min=convertBin(test_thresh, 1);
    max=convertBin(test_thresh, 1024);

    TH1F* th1_thresh_l = new TH1F ("th1_thresh_l", "thresh l>r",  max*2, 0, max);
    TH1F* th1_thresh_r = new TH1F ("th1_thresh_r", "thresh r>l",  max*2, 0, max);

    TH2F* thresholds_l = new TH2F ("th2_thresholds_l" , "thresholds l>r" , 16 , 0 , 16 , 1024 , min , max);
    TH2F* thresholds_r = new TH2F ("th2_thresholds_r" , "thresholds r>l" , 16 , 0 , 16 , 1024 , min , max);

    th1_thresh_l->GetXaxis() -> SetTitle("threshold (mv)");
    th1_thresh_l->GetYaxis() -> SetTitle("counts");
    th1_thresh_r->GetXaxis() -> SetTitle("threshold (mv)");
    th1_thresh_r->GetYaxis() -> SetTitle("counts");

    thresholds_l -> GetYaxis() -> SetTitle("threshold (mV)");
    thresholds_r -> GetYaxis() -> SetTitle("threshold (mV)");

    thresholds_l -> GetXaxis() -> SetTitle("channel");
    thresholds_r -> GetXaxis() -> SetTitle("channel");

    //-OFFSETS----------------------------------------------------------------------------------------------------------

    min=convertBin(test_offset, 1);
    max=convertBin(test_offset, 1024);

    TH1F* th1_offset_l = new TH1F ("th1_offset_l", "offset l>r",  max*2, 0, max);
    TH1F* th1_offset_r = new TH1F ("th1_offset_r", "offset r>l",  max*2, 0, max);

    TH2F* offsets_l    = new TH2F ("th2_offsets_l"    , "offsets l>r"    , 16 , 0 , 16 , 1024 , min , max);
    TH2F* offsets_r    = new TH2F ("th2_offsets_r"    , "offsets r>l"    , 16 , 0 , 16 , 1024 , min , max);

    th1_offset_l->GetXaxis() -> SetTitle("offset (mv)");
    th1_offset_l->GetYaxis() -> SetTitle("counts");
    th1_offset_r->GetXaxis() -> SetTitle("offset (mv)");
    th1_offset_r->GetYaxis() -> SetTitle("counts");

    offsets_l -> GetYaxis() -> SetTitle("offset (mV)");
    offsets_r -> GetYaxis() -> SetTitle("offset (mV)");

    offsets_l -> GetXaxis() -> SetTitle("channel");
    offsets_r -> GetXaxis() -> SetTitle("channel");

    //-REFILLING--------------------------------------------------------------------------------------------------------

    TH2F* raws []      = {offsets_l_raw, offsets_r_raw, thresholds_l_raw, thresholds_r_raw};
    TH2F* converted [] = {offsets_l, offsets_r, thresholds_l, thresholds_r};

    for (int iscan=0; iscan<2; iscan++) {
    for (int iside=0; iside<2; iside++) {

        converted[iscan*2+iside]->GetXaxis()->SetNdivisions(-16);
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(1  , "0");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(2  , "1");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(3  , "2");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(4  , "3");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(5  , "4");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(6  , "5");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(7  , "6");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(8  , "7");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(9  , "8");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(10 , "9");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(11 , "A");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(12 , "B");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(13 , "C");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(14 , "D");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(15 , "E");
        converted[iscan*2+iside]->GetXaxis()->SetBinLabel(16 , "F");

        raws[iscan*2+iside]->GetXaxis()->SetNdivisions(-16);
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(1  , "0");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(2  , "1");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(3  , "2");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(4  , "3");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(5  , "4");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(6  , "5");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(7  , "6");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(8  , "7");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(9  , "8");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(10 , "9");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(11 , "A");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(12 , "B");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(13 , "C");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(14 , "D");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(15 , "E");
        raws[iscan*2+iside]->GetXaxis()->SetBinLabel(16 , "F");

        raws[iscan*2+iside]->SetMaximum(1);
        raws[iscan*2+iside]->SetMinimum(0);

        converted[iscan*2+iside]->SetMaximum(1);
        converted[iscan*2+iside]->SetMinimum(0);


    for (int istrip=0; istrip<16; istrip++) {
    for (int ibin=0;ibin<1024;ibin++) {

            int    strip     = istrip;
            float  errors    = raws[iscan*2+iside]->GetBinContent(istrip+1, ibin+1);

            converted[iscan*2+iside]-> SetBinContent(strip+1, ibin+1, errors);


    } } } }

    // thresholds_l = thresholds_l_raw ;
    // thresholds_r = thresholds_r_raw ;
    // offsets_l    = offsets_l_raw    ;
    // offsets_r    = offsets_r_raw    ;


    //------------------------------------------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------------------------------------------


    thresholds_l -> SetStats(0);
    thresholds_r -> SetStats(0);
    offsets_l -> SetStats(0);
    offsets_r -> SetStats(0);

#ifdef DRAWFITS
    TCanvas * c2 = new TCanvas ();
    c2->SetWindowSize(2400,1280);
    c2->Divide(8,2);

    TCanvas * c3 = new TCanvas ();
    c3->SetWindowSize(2400,1280);
    c3->Divide(8,2);

    TCanvas * c4 = new TCanvas ();
    c4->SetWindowSize(2400,1280);
    c4->Divide(8,2);

    TCanvas * c5 = new TCanvas ();
    c5->SetWindowSize(2400,1280);
    c5->Divide(8,2);
#endif


    gStyle->SetPalette(1);

    c1->cd(1);
    offsets_l->Draw("COLZ");
    offsets_l->SetContour(100);

    c1->cd(2);
    offsets_r->Draw("COLZ");
    offsets_r->SetContour(100);

    c1->cd(3);
    thresholds_l->Draw("COLZ");
    thresholds_l->SetContour(100);

    c1->cd(4);
    thresholds_r->Draw("COLZ");
    thresholds_r->SetContour(100);

    c1->Modified();
    c1->Update();

    // draw now
    gSystem->ProcessEvents();



    c1->cd(5);

    float err_rate [1024];
    float bin [1024];

    float fit_thresh [16];
    float fit_span   [16];
    float fit_width  [16];

    TGraph*  graphs [16];
    TF1*     fits   [16];

    TMarker* marks  [16];


    for (int iscan=0; iscan<2; iscan++) {
    for (int iside=0; iside<2; iside++) {

    TH2F* h2;

    TH1F* h1;

    if (iscan==test_thresh && iside==0) {
        h2 = thresholds_l;
        h1 = th1_thresh_l;
    }
    else if  (iscan==test_thresh && iside==1) {
        h2 = thresholds_r;
        h1 = th1_thresh_r;
    }
    else if (iscan==test_offset && iside==0) {
        h2 = offsets_l;
        h1 = th1_offset_l;
    }
    else if (iscan==test_offset && iside==1) {
        h2 = offsets_r;
        h1 = th1_offset_r;
    }

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

        const char* side_str_short [] = {"l",   "r"  };
        const char* side_str_long  [] = {"l>r", "r>l"};

        char *name;
        char *title;

        asprintf(&name, "%s_%s%d",          testname[iscan], side_str_short[iside], istrip);
        asprintf(&title,"%s %s Channel %d", testname[iscan], side_str_long [iside],  istrip);

        gr->SetName(name);
        gr->SetTitle(title);

        TF1 *f1 = new TF1("fit","[0]/2*(1-TMath::Erf([1]*(x-[2])))", 0, 1024);

        //std::cout << "fitting on strip " << istrip << std::endl;

#ifdef FITS
        if (iscan==test_thresh) {
            f1->SetParameter(0, 1.05);
            f1->SetParameter(1, .3 );
            f1->SetParameter(2, 150);

            gr->Fit("fit", "q", "", 20, 400);
        }
        else if (iscan==test_offset) {
            if (iside==0) { // l>r
                f1->SetParameter(0, 1.1 );
                f1->SetParameter(1, .2   );
                f1->SetParameter(2, 300 );
                gr->Fit("fit", "q", "", 100, 1024);
            }
            else {
                f1->SetParameter(0, 1.1 );
                f1->SetParameter(1, .5   );
                f1->SetParameter(2, 100 );
                gr->Fit("fit", "q", "", 20, 200);
            }
        }
#endif

        float p0 = f1->GetParameter(0);
        float p1 = f1->GetParameter(1);
        float p2 = f1->GetParameter(2);

        // just check whether the fit failed
        bool fail=0;
        if (p0 > 1.1 || p0<0.9) {
            fail=true;
        }
        if (p1>100 || p1<0) {
            fail=true;
        }
        if (p2<0) {
            fail=true;
        }

#ifdef FITS
        if (fail) {
            gr->GetFunction("fit")->SetLineColor(kRed);
            std::cout << "Failed to fit on test=" << std::endl;
            printf("Failed to fit on test=%i, side=%i\n", iscan, iside);
        }
        else {
            gr->GetFunction("fit")->SetLineColor(kGreen+3);
        }
#endif

                         // TF1 *f2  = new TF1("gauussed","2*[0]*[1] / TMath::Sqrt(TMath::Pi()) * (TMath::Exp(  -1*TMath::Power( [1] * (x-[2]) ,2))  )", 0, 1024);
                         // f2->SetNpx(1024);
                         // f1->SetNpx(1024);

                         // f2->SetParameter(0, p0);
                         // f2->SetParameter(1, p1);
                         // f2->SetParameter(2, p2);

        if (fail) {
            fit_span   [istrip] = f1->GetParameter(0);
            fit_width  [istrip] = f1->GetParameter(1);
            fit_thresh [istrip] = 99;;

        }
        else {
            fit_span   [istrip] = f1->GetParameter(0);
            fit_width  [istrip] = f1->GetParameter(1);
            fit_thresh [istrip] = f1->GetParameter(2);
        }

        //--------------------------------------------------------------------------------------------------------------

        double val = convertBin(iscan, fit_thresh[istrip]);

        h1->Fill(val);

#ifdef DRAWFITS
         if (iscan==0 && iside==0) {
             c2->cd(istrip+1);
             gPad->DrawFrame(0,0, 300, 1.1);
             gr->Draw("SAME, *A");
             gr-> SetMarkerStyle(3);
         }
         else if (iscan==0 && iside==1) {
             c3->cd(istrip+1);
             gPad->DrawFrame(0,0, 300, 1.1);
             gr->Draw("SAME, *A");
             gr-> SetMarkerStyle(3);
         }
         else if (iscan==1 && iside==0) {
             c4->cd(istrip+1);
             gPad->DrawFrame(0,0, 300, 1.1);
             gr->Draw("SAME, *A");
             gr-> SetMarkerStyle(3);
         }
         else if (iscan==1 && iside==1) {
             c5->cd(istrip+1);
             gPad->DrawFrame(0,0, 300, 1.1);
             gr->Draw("SAME, *A");
             gr-> SetMarkerStyle(3);
         }
#endif

        c1->cd(2*iscan+iside+1);

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

    c1->cd (7+2*iscan+iside);
    h1->SetFillColor(kBlue-10);
    h1->Draw();
    gSystem->ProcessEvents();

    }
    }

    c1->cd();


}
