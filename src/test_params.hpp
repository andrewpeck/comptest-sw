#ifndef TEST_PARAMS_HPP
#define TEST_PARAMS_HPP

/* Test Parameters */

/* Fraction of Correct Response Required to Pass */
static const double PASS_THRESHOLD   = 0.5;

/* Number of Pulses to Inject at Each Setting */
static const int   NUM_PULSES       = 100;

/* Comparator Threshold Value (in volts) */
static const float CDAC_VALUE       = 120;

/* Min/Max pulse DAC value to scan over */
static const int   PDAC_MIN         = 0;
static const int   PDAC_MAX         = 200;

/* 1 scans every step, 2 skips a step, 3 skips two steps, etc*/
static const int   SCAN_GRANULARITY = 2;

/* Comparator Injection Delay */
static const int   DDD_DELAY        = 50;

/* FPGA Integer BX Delay */
static const int BX_DELAY = 6;

/* Integer Pulse Width */
static const int PULSE_WIDTH = 1;

/* Triad SM Persistance */
static const int   TRIAD_PERSIST    = 0;
static const int   TRIAD_PERSIST1   = 1;

static const int   COMPIN_INJECT    = 0;


/* Scale Factor between Pulse Dac Amplitude and Shaping Circuit Output
 * Amplitude... to be measured empirically */
static const double PULSEAMP_SCALE_FACTOR = 0.20;

/* Attenuation of L R C pulses */
static inline double DB_TO_VOLTAGE(float db) { return pow(10, db/20.);}
static const double ATTENUATION_HIGH = DB_TO_VOLTAGE(-6);  // 0.501187; //  6 dB;
static const double ATTENUATION_LOW  = DB_TO_VOLTAGE(-15); // 0.177828; // 15 dB;
static const double ATTENUATION_MID  = DB_TO_VOLTAGE(-13); // 0.223872; // 13 dB;

static const double REF_THRESH_LOW  = +9.0;
static const double REF_THRESH_HIGH = +16.0;

static const double REF_OFFSET_LOW  = 0.;
static const double REF_OFFSET_HIGH = 1.;

/* Ref=20uA */
static const double REF_IBIAS_LOW = 16.;
static const double REF_IBIAS_HIGH = 24;

/* Ref=10uA */
static const double REF_IAMP_LOW  = 8.;
static const double REF_IAMP_HIGH  = 12.0;

/* Ref=2uA */
static const double REF_IOFF_LOW  = 1.;
static const double REF_IOFF_HIGH  = 3.0;

/* Ref=10uA */
static const double REF_IFAMP_LOW = 8.;
static const double REF_IFAMP_HIGH = 12.0;

/* Ref=? */
static const double REF_I3V3_LOW  = 2.;
static const double REF_I3V3_HIGH  = 8.0;

/* Ref=? */
static const double REF_I5V0_LOW  = 2.;
static const double REF_I5V0_HIGH  = 8.0;

static const double REF_THRESH_DELTA_LOW =0.;
static const double REF_THRESH_DELTA_HIGH=7.;

#endif
