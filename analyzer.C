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
#include "autorange.h"

#define FITS 1
//#define DRAWFITS 1
                  //int argc, char* argv []


void analyzer (std::string directory="./results") {

    TSystemDirectory dir(directory.c_str(), directory.c_str());

    TList *files = dir.GetListOfFiles();

    std::vector<std::string> file_vec;

    if (files) {

        TSystemFile *file;
        TString fname;
        TIter next(files);

        while ((file=(TSystemFile*)next())) {

            fname = file->GetName();

            if (!(file->IsDirectory()) && fname.EndsWith(".root")) {
                std::cout << "    " << std::string(fname.Data()) << std::endl;
                file_vec.push_back(directory+"/"+std::string(fname.Data()));
            }

        }
    }

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

        num_files ++;

        TFile* hfile = new TFile(filename.c_str(),"READ","");

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
    }

    h2_compin  -> GetZaxis() -> SetRangeUser(0,num_files);
    h2_compout -> GetZaxis() -> SetRangeUser(0,num_files);
    //h2_mode    -> GetZaxis() -> SetRangeUser(0,num_files*7);

    gStyle->SetPalette(1);


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

        double xmin = i_mean[channel] - i_spread[channel]*2;
        double xmax = i_mean[channel] + i_spread[channel]*2;

        autoRange (currents_map[channel] , xmin, xmax);

        c1->cd(i);

        currents_map[channel] ->Draw();

        c1->Update();

        xmin = i_mean[channel] - i_spread[channel];
        xmax = i_mean[channel] + i_spread[channel];

        drawLimitLines(gPad, xmin, xmax);

        i++;
    }

    //-THREHSOLDS-------------------------------------------------------------------------------------------------------

    TCanvas * c2 = new TCanvas ();
    c2->SetWindowSize(512*2,512*2);
    c2->Divide(2,2);
    c2->cd(1);
    thresholds_l->Draw("COLZ");
    c2->cd(2);
    thresholds_r->Draw("COLZ");
    c2->cd(3);
    th1_thresh_l -> Draw();
    c2->cd(4);
    th1_thresh_r -> Draw();

    TCanvas * c3 = new TCanvas ();
    c3->SetWindowSize(512*2,512*2);
    c3->Divide(2,2);
    c3->cd(1);
    offsets_l->Draw("COLZ");
    c3->cd(2);
    offsets_r->Draw("COLZ");
    c3->cd(3);
    th1_offset_l -> Draw();
    c3->cd(4);
    th1_offset_r -> Draw();

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
