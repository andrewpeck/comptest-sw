#ifndef DRAWLIMITLINES_H
#define DRAWLIMITLINES_H 

#include <TLine.h>
#include <TPad.h>

inline void drawLimitVlines (TVirtualPad* gpad, double xlow=0, double xhigh=0) {

    double ymax = gpad->GetUymax();

    TLine* llow  = new TLine (xlow,  0, xlow,  ymax);
    TLine* lhigh = new TLine (xhigh, 0, xhigh, ymax);

    llow->SetLineColor(kBlack);
    llow->SetLineStyle(2);
    llow->Draw();

    lhigh->SetLineColor(kBlack);
    lhigh->SetLineStyle(2);
    lhigh->Draw();
}

inline void drawLimitHlines (TVirtualPad* gpad, double ylow=0, double yhigh=0) {

    double xmax = gpad->GetUxmax();

    TLine* llow  = new TLine (0   ,ylow,  xmax, ylow);
    TLine* lhigh = new TLine (0   ,yhigh, xmax, yhigh);

    llow->SetLineColor(kBlack);
    llow->SetLineStyle(2);
    llow->Draw();

    lhigh->SetLineColor(kBlack);
    lhigh->SetLineStyle(2);
    lhigh->Draw();
}

#endif /* DRAWLIMITLINES_H */
