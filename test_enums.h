#ifndef TEST_ENUMS_H
#define TEST_ENUMS_H

enum {test_offset, test_thresh, test_currents, test_timing, test_mode, test_compin, test_compout};

static const char* testname       [] = {"Offset" , "Threshold" , "Current" , "Timing" , "Mode" , "Compin" , "Compout"};
static const char* testname_short [] = {"offset" , "thresh"    , "current" , "timing" , "mode" , "compin" , "compout"};

static const char*  currents       [] = {"ifamp"  , "iamp"      , "ioff"    , "ibias"  , "i5v0" , "i3v3"};
static const char*  current_units  [] = {"uA"     , "uA"        , "uA"      , "uA"     , "mA"   , "mA"};
static const float  current_scale  [] = {1e6      ,  1e6        ,  1e6      , 1e6      , 1e3    , 1e3 };

#endif /* TEST_ENUMS_H */
