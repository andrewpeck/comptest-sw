#include <string>
#include <TROOT.h>
#include <TTree.h>
#include <TBranch.h>
#include <dirent.h>

struct Comparator_currents_t {
    float ibias;
    float iamp;
    float ifamp;
    float ioff;
    float i3v3;
    float i5v0;
};

struct TestResult_t
{
    float  thresh_l [16];
    float  thresh_r [16];
    float  offset_l [15];
    float  offset_r [15];
    struct Comparator_currents_t currents;
};


void generate_tree () {
    float *ibias;
    float *iamp;
    float *ifamp;
    float *ioff;
    float *i3v3;
    float *i5v0;
    float  thresh_l [16];
    float  thresh_r [16];
    float  offset_l [15];
    float  offset_r [15];

    //open tfile
    TFile *outfile = new TFile("results.root", "RECREATE");

    // tree
    TTree *tree = new TTree("T","LCT Comparator Test Results");

    TestResult_t result;

    tree->Branch("ibias", &result.currents.ibias,"ibias/F");
    tree->Branch("iamp",  &result.currents.iamp,  "iamp/F");
    tree->Branch("ifamp", &result.currents.ifamp,"ifamp/F");
    tree->Branch("ioff",  &result.currents.ioff,  "ioff/F");
    tree->Branch("i3v3",  &result.currents.i3v3,  "i3v3/F");
    tree->Branch("i5v0",  &result.currents.i5v0,  "i5v0/F");

    tree->Branch("thresh_l", result.thresh_l,  "thresh_l[16]/F");
    tree->Branch("thresh_r", result.thresh_r,  "thresh_r[16]/F");

    tree->Branch("offset_l", result.offset_l,  "offset_l[15]/F");
    tree->Branch("offset_r", result.offset_r,  "offset_r[15]/F");

    //int split = 1;
    //int bufsize = 64000;
    //tree->Branch("chip", &result, bufsize, split);





    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("../log/raw")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            std::string filename = ent->d_name;
            if (filename=="." || filename=="..")
                continue;
            else {
                filename = "../log/raw/" + filename;
                printf (") %s\n", ent->d_name);

                //open the binary waveform file
                FILE *f;
                cout << filename;
                if ( (f=fopen(filename.c_str(),"rb")) == NULL)
                    cout << "File not found\n";

                fread(&result, sizeof(result), 1,f);
                printf("%f\n", result.currents.ibias);

                tree->Fill();
                fclose(f);
            }
        }
        closedir (dir);
    } else {
        perror ("");
        /* could not open directory */
        //return EXIT_FAILURE;
    }
    tree->Write();
    outfile->Close();
}
