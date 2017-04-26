#ifndef LIMITS_H
#define LIMITS_H 

#include <map>
#include <vector>
#include <string>

static std::vector<std::string> currents_vec = {"iamp", "ioff", "ibias", "ifamp", "i5v0", "i3v3"};

static std::map <std::string, float> i_mean = {
        {"iamp"  , 48.77 },
        {"ioff"  , 7.3   },
        {"ibias" , 6.831 },
        {"ifamp" , 10.27 },
        {"i5v0"  , 41.8  },
        {"i3v3"  , 20.7  },
    };

static std::map <std::string, float> i_spread = {
        {"iamp"   , 0.5 },
        {"ioff"   , 0.1 },
        {"ibias"  , 0.09},
        {"ifamp"  , 0.07},
        {"i5v0"   , 3.0 },
        {"i3v3"   , 2.0 },
    };

#endif /* LIMITS_H */
