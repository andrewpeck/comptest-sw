#ifndef BOARD_CHARACTERISTICS_H
#define BOARD_CHARACTERISTICS_H

#include <cmath>
#include "test_enums.h"
#include <stdint.h>

static const int dac_start   = 20;
static const int dac_step    = 1;
static const int num_pulses  = 10;
static const int num_entries = 1024;

static const int dac_start_thresh   = 10;
static const int dac_step_thresh    = 1;

static const int dac_start_offset   = 10;
static const int dac_step_offset    = 1;

static const int dac_start_compout   = 10;
static const int dac_step_compout    = 10;

static const float pulse_vref = 5.0;

static const float shaping_scale_factor = 0.347;

static const float attenuation_high  = 1./pow(10, 6.0/20); // 0.501
static const float attenuation_med   = 1./pow(10,11.0/20); // 0.282
static const float attenuation_low   = 1./pow(10,13.0/20); // 0.224


static const float r_ifamp = 390000;
static const float r_iamp  = 75000;
static const float r_ibias = 130000;
static const float r_ioff  = 510000;

static const float r_ps3v3 = 0.01;
static const float r_ps5v0 = 0.01;

static const float ps3v3_gain_stage_1 = 10000/100.;
static const float ps5v0_gain_stage_1 = 10000/100.;

static const float ps3v3_gain_stage_2 = 1+10000./1000;
static const float ps5v0_gain_stage_2 = 1+10000./1000;

inline uint16_t getDacStartStep (int scan, int startstep) {

    uint16_t start=0;
    uint16_t step=0;

    if (scan==test_thresh) {
        start=dac_start_thresh;
        step=dac_step_thresh;
    }
    else if (scan==test_offset) {
        start=dac_start_offset;
        step=dac_step_offset;
    }
    else if (scan==test_compin) {
        start=dac_start_thresh;
        step=dac_step_thresh;
    }
    else if (scan==test_compout) {
        start=dac_start_thresh;
        step=dac_step_thresh;
    }

    if (startstep)
        return step;
    else
        return start;

}
#endif /* BOARD_CHARACTERISTICS_H */
