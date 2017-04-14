
#include "test_enums.h"
#include "board_characteristics.h"
#include "data.cpp"
#include <TH2F.h>
#include <TFile.h>

void show_plots () {

    TFile* hfile = new TFile("tmp.root","READ","TMB Queue Model");

    TH2F* thresholds_l_raw = (TH2F*) hfile -> Get ("thresholds_l");
    TH2F* thresholds_r_raw = (TH2F*) hfile -> Get ("thresholds_r");
    TH2F* offsets_l_raw    = (TH2F*) hfile -> Get ("offsets_l");
    TH2F* offsets_r_raw    = (TH2F*) hfile -> Get ("offsets_r");

    TH1F* th1_thresh_l = new TH1F ("th1_thresh_l", "thresh l>r",  60, 0, 30);
    TH1F* th1_thresh_r = new TH1F ("th1_thresh_r", "thresh r>l",  60, 0, 30);

    TH1F* th1_offset_l = new TH1F ("th1_offset_l", "offset l>r",  50, 0, 5);
    TH1F* th1_offset_r = new TH1F ("th1_offset_r", "offset r>l",  50, 0, 5);

    float max;
    float min;

    min=convertBin(test_thresh, 0);
    max=convertBin(test_thresh, 1024);

    TH2F* thresholds_l = new TH2F ("th2_thresholds_l" , "thresholds_l" , 16 , 0 , 16 , 1024 , min , max);
    TH2F* thresholds_r = new TH2F ("th2_thresholds_r" , "thresholds_r" , 16 , 0 , 16 , 1024 , min , max);

    min=convertBin(test_offset, 0);
    max=convertBin(test_offset, 1024);

    TH2F* offsets_l    = new TH2F ("th2_offsets_l"    , "offsets_l"    , 16 , 0 , 16 , 1024 , min , max);
    TH2F* offsets_r    = new TH2F ("th2_offsets_r"    , "offsets_r"    , 16 , 0 , 16 , 1024 , min , max);


    TH2F* raws []      = {offsets_l_raw, offsets_r_raw, thresholds_l_raw, thresholds_r_raw};
    TH2F* converted [] = {offsets_l, offsets_r, thresholds_l, thresholds_r};

    for (int iscan=0; iscan<2; iscan++) {
    for (int iside=0; iside<2; iside++) {
    for (int istrip=0; istrip<16; istrip++) {
    for (int ibin=0;ibin<1024;ibin++) {

            int    strip     = istrip;
            float  amplitude = ibin; // convertBin(iscan, ibin);
            float  errors    = raws[iscan*2+iside]->GetBinContent(istrip+1, ibin+1);

            converted[iscan*2+iside]->
                SetBinContent(strip+1, amplitude+1, errors);

    } } } }

//    thresholds_l = thresholds_l_raw ;
//    thresholds_r = thresholds_r_raw ;
//    offsets_l    = offsets_l_raw    ;
//    offsets_r    = offsets_r_raw    ;


    //------------------------------------------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------------------------------------------


    thresholds_l -> SetStats(0);
    thresholds_r -> SetStats(0);
    offsets_l -> SetStats(0);
    offsets_r -> SetStats(0);

    TCanvas * c2 = new TCanvas ();
    c2->SetWindowSize(2400,1280);
    c2->Divide(8,2);

    TCanvas * c1 = new TCanvas ();
    c1->SetWindowSize(1600,1024);
    c1->Divide(4,2);
    c1->cd();


    gStyle->SetPalette(1);

    c1->cd(3);
    thresholds_l->Draw("COLZ");
    thresholds_l->SetContour(100);
    c1->cd(4);
    thresholds_r->Draw("COLZ");
    thresholds_r->SetContour(100);
    c1->cd(1);
    offsets_l->Draw("COLZ");
    offsets_l->SetContour(100);
    c1->cd(2);
    offsets_r->Draw("COLZ");
    offsets_r->SetContour(100);

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

        f1->SetParameter(0, 0.99);
        f1->SetParameter(1, 3);
        f1->SetParameter(2, 150);

        //std::cout << "fitting on strip " << istrip << std::endl;

        gr->Fit("fit", "", "", (iscan==test_thresh) ? 5 : 5, 800);

        float p0 = f1->GetParameter(0);
        float p1 = f1->GetParameter(1);
        float p2 = f1->GetParameter(2);

        // just check whether the fit failed
        bool fail=0;
        if (p0 > 1.1 || p0<0.9)
            fail=true;
        if (p1>100 || p1<0)
            fail=true;
        if (p2<0)
            fail=true;

        if (fail) {
            gr->GetFunction("fit")->SetLineColor(kRed);
            std::cout << "FAIL" << std::endl;
        }
        else {
            gr->GetFunction("fit")->SetLineColor(kGreen);
        }

                         // TF1 *f2  = new TF1("gauussed","2*[0]*[1] / TMath::Sqrt(TMath::Pi()) * (TMath::Exp(  -1*TMath::Power( [1] * (x-[2]) ,2))  )", 0, 1024);
                         // f2->SetNpx(1024);
                         // f1->SetNpx(1024);

                         // f2->SetParameter(0, p0);
                         // f2->SetParameter(1, p1);
                         // f2->SetParameter(2, p2);

        fit_span   [istrip] = f1->GetParameter(0);
        fit_width  [istrip] = f1->GetParameter(1);
        fit_thresh [istrip] = f1->GetParameter(2);

        //--------------------------------------------------------------------------------------------------------------

        double val = convertBin(iscan, fit_thresh[istrip]);

        h1->Fill(val);

        if (iscan==0 && iside==0) {
            c2->cd(istrip+1);
            gPad->DrawFrame(0,0, 300, 1.1);
            gr->Draw("SAME, *A");
            gr-> SetMarkerStyle(3);
        }

        c1->cd(2*iscan+iside+1);

        TMarker * mark = new TMarker (0.5+double(istrip), val, 3);

        if (fail)
            mark->SetY (0);

        mark->Draw("SAME");

    }

    c1->cd (5+2*iscan+iside);
    h1->Draw();

    }
    }

    c1->cd();
}
