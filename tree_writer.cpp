// #include <stdint.h>

// #include "tree_writer.h"
// #include "strip_sides.h"
// #include "test_enums.h"
//
//
// void treeWriter::spawnTrees () {
//
//     TString filename = "output.root";
//
//     hfile = new TFile(filename,"RECREATE","Demo ROOT file with histograms");
//
//     thresholds_r = new TTree("Thresholds_r>l","Thresholds r>l");
//     thresholds_l = new TTree("Thresholds_l>r","Thresholds l>r");
//     offsets_r    = new TTree("Offsets_r>l","Offsets r>l");
//     offsets_l    = new TTree("Offsets_l>r","Offsets l>r");
//     currents     = new TTree("Currents","Currents");
//
// //    thresholds_r->Write();
// //    thresholds_l->Write();
// //    offsets_r->Write();
// //    offsets_l->Write();
// //    currents->Write();
//
// }
//
//
// void treeWriter::fillThresh (int strip, int side, uint16_t* data) {
//     fillGeneric (test_thresh, strip, side, data);
// }
//
// void treeWriter::fillOffset (int strip, int side, uint16_t* data) {
//     fillGeneric (test_offset, strip, side, data);
// }
//
// void treeWriter::fillGeneric (int scan, int strip, int side, uint16_t* data) {
//
//     uint16_t data_entry;
//
//     TTree *tree;
//
//     if (scan==test_thresh) {
//         if      (side==LEFT)  tree = thresholds_l;
//         else if (side==RIGHT) tree = thresholds_r;
//     }
//     else if (scan==test_offset) {
//         if      (side==LEFT)  tree = offsets_l;
//         else if (side==RIGHT) tree = offsets_r;
//     }
//
//     char *branchname; // e.g. l0, l1, l2... r0, r1, r2 etc.
//     char *leaflist;
//
//     const char* side_str_short [] = {"l", "r"};
//
//     asprintf(&branchname, "%s%d", side_str_short[side], strip);
//     asprintf(&leaflist,   "%s%d/s", side_str_short[side], strip);
//
//     TBranch *branch = tree->Branch(branchname, &data_entry, leaflist);
//
//     tree->SetBranchAddress(branchname, &data_entry);
//
//     int nentries = 1024; // tree->GetEntries();
//
//     for (int i=0;i<nentries;i++) {
//         tree->GetEntry(i);
//         data_entry = (int) data[i];
//         branch->Fill();
//     }
//
//     tree->Print();
//     tree->Write();
// }
//
// // TCanvas *canvas = new TCanvas(
// //         "canvas",                      // name
// //         "LCT Comparator Measurements", // title
// //
// //         // pixel coordinates of the top LEFT corner of the canvas
// //         10, // wtopx  -- top x position
// //         10, // wtopy  -- top y position
// //
// //         // canvas size in x and y
// //         1280, // ww
// //         1024   // wh
// //         );
// //
// // canvas->Divide(3,2);
// //
// // TRandom3 random;
// //
// // int Amplitude;
// // int Errorcount;
// //
// // //------------------------------------------------------------------------------------------------------------------
// // // RIGHT thresholds
// // //------------------------------------------------------------------------------------------------------------------
// //
// // canvas->cd(1);
// //
// // TTree *thresholds_r = new TTree("Thresholds_r>l","Thresholds r>l");
// //
// // // https://root.cern/9-fitting-coding
// //
// // thresholds_r->Branch("Amplitude"     , &Amplitude   , "Amplitude/I") ;
// // thresholds_r->Branch("Errorcount"    , &Errorcount,  "Errorcount/I" ) ;
// //
// // for (uint16_t i=0; i<1024; i++) {
// //     Amplitude = i;
// //     Errorcount = random.Gaus (0, 25)+ ((1000 + random.Gaus(0,25))*(1+(-1)*TMath::Erf( (double(i)-512+random.Gaus (0, 25))/64)));
// //
// //     thresholds_r->Fill();
// // }
// //
// // thresholds_r->Write();
// //
// // thresholds_r->Draw("Errorcount:Amplitude");
// //
// // //------------------------------------------------------------------------------------------------------------------
// // // LEFT Thresholds
// // //------------------------------------------------------------------------------------------------------------------
// //
// // canvas->cd(2);
// //
// // TTree *thresholds_l = new TTree("Thresholds_l>l","Thresholds r>l");
// //
// // thresholds_l->Branch("Amplitude"     , &Amplitude   , "Amplitude/I") ;
// // thresholds_l->Branch("Errorcount"    , &Errorcount,  "Errorcount/I" ) ;

void tree_writer() {
    TRandom3 random;
    int Errorcount;
 for (uint16_t i=0; i<1024; i++) {
     //if (i%10==0) {
     Errorcount = 1000* (1+(-1)*TMath::Erf( (double(i)-512)/64));
     printf("%i\n", Errorcount);
     //}
 }
}
//
//     thresholds_l->Fill();
// }
//
// thresholds_l->Write();
//
// thresholds_l->Draw("Errorcount:Amplitude");
//
// delete hfile;
//
//
// }
