#include "test_enums.h"
#include "board_characteristics.h"
#include "limits.h"
#include "drawLimitLines.h"
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
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TRegexp.h>
#include "autorange.h"

#define FITS 1
//#define DRAWFITS 1
                  //int argc, char* argv []


void analyzer (std::string path="./results/*") {

    int fname_start = path.find_last_of("\\/");

    std::cout << "directory=" << path.substr(0, fname_start+1) << std::endl;
    std::cout << "mask     =" << path.substr(fname_start+1) << std::endl;

    std::string directory = path.substr(0,fname_start+1);
    std::string file      = path.substr(fname_start+1);

    TRegexp fexp (file.c_str(), true);
    //TRegexp fexp ("def", true);
    //TRegexp fexp ("*", true);

    std::cout << path.substr(fname_start+1) << std::endl;
    std::cout << path.substr(fname_start+1) << std::endl;

    TSystemDirectory dir(directory.c_str(), directory.c_str());

    TList *files = dir.GetListOfFiles();

    std::vector<std::string> file_vec;

    if (files) {

        TSystemFile *file;
        TString fname;
        TIter next(files);

        while ((file=(TSystemFile*)next())) {

            fname = file->GetName();

            std::cout << fname << std::endl;

            if (!(file->IsDirectory()) && fname.EndsWith(".root") && fname.Contains(fexp)) {
                std::cout << "    " << std::string(fname.Data()) << std::endl;
                file_vec.push_back(directory+"/"+std::string(fname.Data()));
            }

        }
    }

    if (file_vec.size() <1)
        return;

    TH1F* iamp  ;
    TH1F* ioff  ;
    TH1F* ibias ;
    TH1F* ifamp ;
    TH1F* i5v0  ;
    TH1F* i3v3  ;

    TH2F* thresholds_l  ;
    TH2F* thresholds_r  ;

    TH2F* offsets_l  ;
    TH2F* offsets_r  ;

    TH2F* h2_timing  ;
    TH2F* h2_compin  ;
    TH2F* h2_compout  ;
    TH2F* h2_mode  ;

    TH1F* th1_offset_l;
    TH1F* th1_offset_r;

    TH1F* th1_thresh_l;
    TH1F* th1_thresh_r;

    TFile* outfile = new TFile("summary.root","RECREATE","LCT Comparator Test Results");

    //------------------------------------------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------------------------------------------

    std::vector<TH1F*> currents_v = {iamp, ioff, ibias, ifamp, i5v0, i3v3};

    bool first=1;
    int num_files = 0;
    for(auto const& filename: file_vec) {

        std::cout << "Processing histograms of " << filename << std::endl;
        num_files ++;

        TFile* hfile = new TFile(filename.c_str(),"READ","");

        if (hfile<=0) {
            printf("no file\r\n");
            break;
        }

        if (first) {

            first=0;

            th1_offset_l  = (TH1F*) hfile -> Get ("th1_offset_l") -> Clone();
            th1_offset_l  ->SetDirectory(outfile->GetDirectory(""));

            th1_offset_r  = (TH1F*) hfile -> Get ("th1_offset_r") -> Clone();
            th1_offset_r  ->SetDirectory(outfile->GetDirectory(""));

            th1_thresh_l  = (TH1F*) hfile -> Get ("th1_thresh_l") -> Clone();
            th1_thresh_l  ->SetDirectory(outfile->GetDirectory(""));

            th1_thresh_r  = (TH1F*) hfile -> Get ("th1_thresh_r") -> Clone();
            th1_thresh_r  ->SetDirectory(outfile->GetDirectory(""));

            thresholds_l  = (TH2F*) hfile -> Get ("thresholds_l") -> Clone();
            thresholds_l  ->SetDirectory(outfile->GetDirectory(""));

            thresholds_r  = (TH2F*) hfile -> Get ("thresholds_r") -> Clone();
            thresholds_r  ->SetDirectory(outfile->GetDirectory(""));

            offsets_l  = (TH2F*) hfile -> Get ("offsets_l") -> Clone();
            offsets_l  ->SetDirectory(outfile->GetDirectory(""));

            offsets_r  = (TH2F*) hfile -> Get ("offsets_r") -> Clone();
            offsets_r  ->SetDirectory(outfile->GetDirectory(""));

            iamp  = (TH1F*) hfile -> Get ("iamp") -> Clone();
            iamp  ->SetDirectory(outfile->GetDirectory(""));

            iamp  = (TH1F*) hfile -> Get ("iamp") -> Clone();
            iamp  ->SetDirectory(outfile->GetDirectory(""));

            ioff  = (TH1F*) hfile -> Get ("ioff") -> Clone();
            ioff  ->SetDirectory(outfile->GetDirectory(""));

            ibias  = (TH1F*) hfile -> Get ("ibias") -> Clone();
            ibias  ->SetDirectory(outfile->GetDirectory(""));

            ifamp  = (TH1F*) hfile -> Get ("ifamp") -> Clone();
            ifamp  ->SetDirectory(outfile->GetDirectory(""));

            i5v0  = (TH1F*) hfile -> Get ("i5v0") -> Clone();
            i5v0  ->SetDirectory(outfile->GetDirectory(""));

            i3v3  = (TH1F*) hfile -> Get ("i3v3") -> Clone();
            i3v3  ->SetDirectory(outfile->GetDirectory(""));

            h2_timing  = (TH2F*) hfile -> Get ("h2_timing") -> Clone();
            h2_timing  ->SetDirectory(outfile->GetDirectory(""));

            h2_compin  = (TH2F*) hfile -> Get ("h2_compin") -> Clone();
            h2_compin  ->SetDirectory(outfile->GetDirectory(""));

            h2_compout  = (TH2F*) hfile -> Get ("h2_compout") -> Clone();
            h2_compout  ->SetDirectory(outfile->GetDirectory(""));

            h2_mode  = (TH2F*) hfile -> Get ("h2_mode") -> Clone();
            h2_mode  ->SetDirectory(outfile->GetDirectory(""));
        }

        else {
            iamp         -> Add((TH1F*) hfile -> Get ("iamp"));
            ioff         -> Add((TH1F*) hfile -> Get ("ioff"));
            ibias        -> Add((TH1F*) hfile -> Get ("ibias"));
            ifamp        -> Add((TH1F*) hfile -> Get ("ifamp"));
            i5v0         -> Add((TH1F*) hfile -> Get ("i5v0"));
            i3v3         -> Add((TH1F*) hfile -> Get ("i3v3"));

            thresholds_l -> Add((TH2F*) hfile -> Get ("thresholds_l"));
            thresholds_r -> Add((TH2F*) hfile -> Get ("thresholds_r"));

            offsets_l    -> Add((TH2F*) hfile -> Get ("offsets_l"));
            offsets_r    -> Add((TH2F*) hfile -> Get ("offsets_r"));

            h2_timing    -> Add((TH2F*) hfile -> Get ("h2_timing"));
            h2_compout   -> Add((TH2F*) hfile -> Get ("h2_compout"));
            h2_mode      -> Add((TH2F*) hfile -> Get ("h2_mode"));
            h2_compin    -> Add((TH2F*) hfile -> Get ("h2_compin"));

            th1_thresh_l -> Add((TH2F*) hfile -> Get ("th1_thresh_l"));
            th1_thresh_r -> Add((TH2F*) hfile -> Get ("th1_thresh_r"));
            th1_offset_l -> Add((TH2F*) hfile -> Get ("th1_offset_l"));
            th1_offset_r -> Add((TH2F*) hfile -> Get ("th1_offset_r"));

        }

        if ( ((TH1F*) hfile->Get("th1_thresh_l"))->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_thresh_l"))->FindFirstBinAbove(0,1)-1)   < mean["thresh_l"] - spread["thresh_l"] ||
             ((TH1F*) hfile->Get("th1_thresh_l"))->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_thresh_l"))->FindLastBinAbove (0,1)+1)   > mean["thresh_l"] + spread["thresh_l"])
            printf("Bin out of limit on th1_thresh_l, %s\n", filename.c_str());
        if ( th1_thresh_r->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_thresh_r"))->FindFirstBinAbove(0,1)-1)   < mean["thresh_r"] - spread["thresh_r"] ||
             ((TH1F*) hfile->Get("th1_thresh_r"))->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_thresh_r"))->FindLastBinAbove (0,1)+1)   > mean["thresh_r"] + spread["thresh_r"])
            printf("Bin out of limit on th1_thresh_r, %s\n", filename.c_str());
        if ( th1_offset_l->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_offset_l"))->FindFirstBinAbove(0,1)-1)   < mean["offset_l"] - spread["offset_l"] ||
             ((TH1F*) hfile->Get("th1_offset_l"))->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_offset_l"))->FindLastBinAbove (0,1)+1)   > mean["offset_l"] + spread["offset_l"])
            printf("Bin out of limit on th1_offset_l, %s\n", filename.c_str());
        if ( th1_offset_r->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_offset_r"))->FindFirstBinAbove(0,1)-1)   < mean["offset_r"] - spread["offset_r"] ||
             ((TH1F*) hfile->Get("th1_offset_r"))->GetXaxis()->GetBinCenter(((TH1F*) hfile->Get("th1_offset_r"))->FindLastBinAbove (0,1)+1)   > mean["offset_r"] + spread["offset_r"])
            printf("Bin out of limit on th1_offset_r, %s\n", filename.c_str());

        hfile->Close();
    }

    h2_compin  -> GetZaxis() -> SetRangeUser(0,num_files);
    h2_compout -> GetZaxis() -> SetRangeUser(0,num_files);
    //h2_mode    -> GetZaxis() -> SetRangeUser(0,num_files*7);

    gStyle->SetPalette(1);

    //const int NRGBs = 4, NCont = 100;
    //gStyle->SetNumberContours(NCont);
    ////                        white -->
    //Double_t stops [NRGBs] = { 0.25, 0.45, 0.60, 0.75};
    //Double_t red   [NRGBs] = { 0.00, 0.00, 0.97, 0.97};
    //Double_t green [NRGBs] = { 0.30, 0.40, 0.97, 0.00};
    //Double_t blue  [NRGBs] = { 0.97, 0.00, 0.00, 0.00};

    //TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);


    //-CURRENTS---------------------------------------------------------------------------------------------------------

    TCanvas * c1 = new TCanvas ();
    c1->SetWindowSize(512*3,512*2);
    c1->Divide(3,2);

    std::map <std::string, TH1F*> currents_map;

    currents_map ["iamp"]  = iamp;
    currents_map ["ioff"]  = ioff;
    currents_map ["ibias"] = ibias;
    currents_map ["ifamp"] = ifamp;
    currents_map ["i5v0"]  = i5v0;
    currents_map ["i3v3"]  = i3v3;

    int i=1;
    for(auto& channel : currents_vec) {

        double xmin = mean[channel] - spread[channel]*2;
        double xmax = mean[channel] + spread[channel]*2;

        autoRange (currents_map[channel] , xmin, xmax);

        c1->cd(i);

        TH1F* h1 = currents_map[channel];

        h1 ->Draw();

        c1->Update();

        xmin = mean[channel] - spread[channel];
        xmax = mean[channel] + spread[channel];


        drawLimitVlines (gPad, xmin, xmax);

        i++;
    }

    //-THREHSOLDS-------------------------------------------------------------------------------------------------------

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

    TCanvas * c2 = new TCanvas ();
    c2->SetWindowSize(512*2,512*2);
    c2->Divide(2,2);

    TCanvas * c3 = new TCanvas ();
    c3->SetWindowSize(512*2,512*2);
    c3->Divide(2,2);

    int ipad=1;
    for (auto & channel : meas_vec) {
        TCanvas* c;
        if (channel=="thresh_r" || channel=="thresh_l")
            c = c2;
        else
            c = c3;

        TH2F* h2 = meas_map[channel];
        TH1F* h1 = meas_map_h1[channel];

        c->cd (ipad);

        h2 -> SetOption("COLZ");
        h2->Draw();
        h2->SetContour(100);
        c->Update();

        double ymin = mean[channel] - spread[channel];
        double ymax = mean[channel] + spread[channel];
        drawLimitHlines(gPad, ymin, ymax);

        c->cd (ipad+2);

        h1->Draw();
        h1->SetContour(100);
        c->Update();
        drawLimitVlines(gPad, ymin, ymax);

        ipad++;
        if (ipad==3)
            ipad=1;
    }

    // draw now
    gSystem->ProcessEvents();

    //-MISC-------------------------------------------------------------------------------------------------------------

    TCanvas * c4 = new TCanvas ();
    c4->SetWindowSize(512*2,512*2);
    c4->Divide(2,2);
    c4->cd(1);
    h2_timing->Draw("COLZ");
    c4->cd(2);
    h2_compout->Draw("COLZ");
    c4->cd(3);
    h2_compin->Draw("COLZ");
    c4->cd(4);
    h2_mode->Draw("COLZ");


    outfile->Write();
}
