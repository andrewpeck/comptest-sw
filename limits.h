#ifndef LIMITS_H
#define LIMITS_H

#include <map>
#include <vector>
#include <string>

static std::vector<std::string> currents_vec = {"iamp", "ioff", "ibias", "ifamp", "i5v0", "i3v3"};
static std::vector<std::string> meas_vec     = {"offset_l", "offset_r", "thresh_l", "thresh_r"};

static std::map <std::string, float> mean = {
        {"iamp"  , 48    }, // 48.77 for 2001 chips
        {"ioff"  , 7.225 },
        {"ibias" , 6.728 },
        {"ifamp" , 10.13 },
        {"i5v0"  , 40.3  },
        {"i3v3"  , 20.4  },
        {"thresh_l", 7.5},
        {"thresh_r", 7.5},
        {"offset_l", 2.0},
        {"offset_r", 0.6},
};

static std::map <std::string, float> spread = {
        {"iamp"   , 0.9 }, // microamp
        {"ioff"   , 0.08}, // microamp
        {"ibias"  , 0.15},
        {"ifamp"  , 0.18},
        {"i5v0"   , 3.0 },
        {"i3v3"   , 2.0 },
        {"thresh_l" , 5},
        {"thresh_r" , 5},
        {"offset_l", 2.8},
        {"offset_r", 0.6},
};

#endif /* LIMITS_H */
