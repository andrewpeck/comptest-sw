#ifndef TEST_PARAMS_HPP
#define TEST_PARAMS_HPP

/* Test Parameters */
static const double PASS_THRESHOLD   = 0.5;
static const int   NUM_PULSES       = 1000;
static const int   CDAC_VALUE       = 120;
static const int   PDAC_MIN         = 0;
static const int   PDAC_MAX         = 200;
static const int   SCAN_GRANULARITY = 2;
static const int   DDD_DELAY        = 0;
static const int   TRIAD_PERSIST    = 0;
static const int   TRIAD_PERSIST1   = 1;
static const int   COMPIN_INJECT    = 0;


static const double PULSEAMP_SCALE_FACTOR = 0.20;
static const double ATTENUATION_HIGH = 0.501187; //  6 dB;
static const double ATTENUATION_LOW  = 0.177828; // 15 dB;
static const double ATTENUATION_MID  = 0.223872; // 13 dB;

static const double REF_THRESH_LOW  = +9.0;
static const double REF_THRESH_HIGH = +16.0;

static const double REF_OFFSET_LOW  = 0.;
static const double REF_OFFSET_HIGH = 1.;

/* Ref==20uA */
static const double REF_IBIAS_LOW = 16.;
static const double REF_IBIAS_HIGH = 24;

/* Ref=10uA */
static const double REF_IAMP_LOW  = 8.;
static const double REF_IAMP_HIGH  = 12.0;

/* Ref==2uA */
static const double REF_IOFF_LOW  = 1.;
static const double REF_IOFF_HIGH  = 3.0;

/* Ref 10uA */
static const double REF_IFAMP_LOW = 8.;
static const double REF_IFAMP_HIGH = 12.0;

/* Ref== */
static const double REF_I3V3_LOW  = 2.;
static const double REF_I3V3_HIGH  = 8.0;

/* Ref */
static const double REF_I5V0_LOW  = 2.;
static const double REF_I5V0_HIGH  = 8.0;

static const double REF_THRESH_DELTA_LOW =0.;
static const double REF_THRESH_DELTA_HIGH=7.;

#endif
