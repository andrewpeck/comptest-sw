#ifndef TEST_ENUMS_H
#define TEST_ENUMS_H

enum {test_offset, test_thresh, test_currents, test_timing};

static const char* testname       [] = {"Offset", "Threshold", "Current", "Timing"};
static const char* testname_short [] = {"offset", "thresh", "current", "timing"};
static const char* currents       [] = {"+ifamp", "+iamp", "+ioff", "+ibias", "+5v0", "+3v3"};
static const char* current_units  [] = {"mA", "mA", "mA", "mA", "mA", "mA"};

#endif /* TEST_ENUMS_H */
