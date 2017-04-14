

void show_plots () {

    TFile* hfile = new TFile("tmp.root","READ","TMB Queue Model");

    TH2F* thresholds_l = (TH2F*) hfile -> Get ("thresholds_l");
    TH2F* thresholds_r = (TH2F*) hfile -> Get ("thresholds_r");
    TH2F* offsets_l = (TH2F*) hfile -> Get ("offsets_l");
    TH2F* offsets_r = (TH2F*) hfile -> Get ("offsets_r");

    thresholds_l -> SetStats(0);
    thresholds_r -> SetStats(0);
    offsets_l -> SetStats(0);
    offsets_r -> SetStats(0);

    TCanvas * c1 = new TCanvas ();
    c1->SetWindowSize(1600,1024);

    c1->Divide(2,2);

    gStyle->SetPalette(1);

    c1->cd(1);
    thresholds_l->Draw("COLZ");
    c1->cd(2);
    thresholds_r->Draw("COLZ");
    c1->cd(3);
    offsets_l->Draw("COLZ");
    c1->cd(4);
    offsets_r->Draw("COLZ");

}
