#include <string>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>

struct Comparator_currents_t {
    double ibias;
    double iamp;
    double ifamp;
    double ioff;
    double i3v3;
    double i5v0;
};

struct TestResult_t
{
    double  thresh_l [16];
    double  thresh_r [16];
    double  offset_l [15];
    double  offset_r [15];
    struct Comparator_currents_t currents;
    double thresh_delta;
};

void generate_tree () {
    double *ibias;
    double *iamp;
    double *ifamp;
    double *ioff;
    double *i3v3;
    double *i5v0;
    double  thresh_l [16];
    double  thresh_r [16];
    double  offset_l [15];
    double  offset_r [15];
    double *thresh_delta;

    //open tfile
    TFile *outfile = new TFile("results.root", "RECREATE");

    // tree
    TTree *tree = new TTree("Results","LCT Comparator Test Results");

    TestResult_t result;

    tree->Branch("ibias", &result.currents.ibias,"ibias/D");
    tree->Branch("iamp",  &result.currents.iamp,  "iamp/D");
    tree->Branch("ifamp", &result.currents.ifamp,"ifamp/D");
    tree->Branch("ioff",  &result.currents.ioff,  "ioff/D");
    tree->Branch("i3v3",  &result.currents.i3v3,  "i3v3/D");
    tree->Branch("i5v0",  &result.currents.i5v0,  "i5v0/D");

    tree->Branch("thresh_l", result.thresh_l,  "thresh_l[16]/D");
    tree->Branch("thresh_r", result.thresh_r,  "thresh_r[16]/D");

    tree->Branch("delta_thresh", &result.thresh_delta, "thresh_delta/D");

    tree->Branch("offset_l", result.offset_l,  "offset_l[15]/D");
    tree->Branch("offset_r", result.offset_r,  "offset_r[15]/D");

    std::string dirname= "../log/raw/";
    TSystemDirectory dir(dirname.c_str(), dirname.c_str());
    TList *files = dir.GetListOfFiles();
    if (files) {
        TSystemFile *file;
        TString fname;
        TIter next(files);
        while ((file=(TSystemFile*)next())) {
            fname = file->GetName();
            if (!file->IsDirectory() && fname.EndsWith(".dat")) {

                std::string filename =dirname + fname.Data();
                FILE *f;
                if ( (f=fopen(filename.c_str(),"rb")) == NULL) {
                    printf("File not found\n");
                }

                fread(&result, sizeof(result), 1,f);
                tree->Fill();
                fclose(f);
            }
        }
    }

    tree->Write();
    outfile->Close();
}
