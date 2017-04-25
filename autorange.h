#ifndef AUTORANGE_H
#define AUTORANGE_H 

#include <TH1F.h>

inline void autoRange (TH1F* h1, double min=std::numeric_limits<double>::max(), double max=0) {
    double first=0;
    double last=0;

    first =1/1.02 * h1->GetXaxis()->GetBinCenter(h1->FindFirstBinAbove(0,1));

    last = 1.02 * h1->GetXaxis()->GetBinCenter(h1->FindLastBinAbove(0,1));

    if (first>min)
        first = min;

    if (last<max)
        last = max;


    h1->GetXaxis()->SetRangeUser(first,last);

}

#endif /* AUTORANGE_H */
