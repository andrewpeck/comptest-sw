#ifndef TEST_ENUMS_H
#define TEST_ENUMS_H

enum {test_offset, test_thresh, test_currents};

static const char* testname       [] = {"Threshold", "Offset", "Current"};
static const char* testname_short [] = {"offset", "thresh", "current"};
static const char* currents []       = {"+ifamp", "+iamp", "+ioff", "+ibias", "+5v0", "+3v3"};

#endif /* TEST_ENUMS_H */
