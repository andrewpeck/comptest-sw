#include "test_enums.h"
#include "board_characteristics.h"
#include <stdint.h>

void convertCounts (float* data, int num_entries, int full_scale) {
    // we read in error counts and we want to normalize that to the number of pulses, producing a sigmoid efficiency curve
    for (int i=0; i<num_entries; i++) {
        data[i] = (data[i] / full_scale);
    }
}

float convertBin (int test, int bin) {

        uint16_t start;
        uint16_t step;

        if (test==test_offset) {
            start=dac_start_offset;
            step=dac_step_offset;
        }
        else {
            start=dac_start_thresh;
            step=dac_step_thresh;
        }

        uint16_t dac_value   = start + bin*step;

        float dac_millivolts = 1000. * pulse_vref * dac_value / 16383.;
        float pulse_voltage  = dac_millivolts * shaping_scale_factor;


        float  hi_amplitude = pulse_voltage * attenuation_high;
        float med_amplitude = pulse_voltage * attenuation_med;
        float low_amplitude = pulse_voltage * attenuation_low;

        if (test==test_thresh) {
            return hi_amplitude;
        }

        else if (test==test_offset) {
            float amp = (med_amplitude - low_amplitude)/3.5;
            return amp ; // 3.5==amp gain
        }
        else {
            return 0;
        }
}

void convertAmplitudes (int test, int dac_start, int dac_step, float* amplitude, int num_entries) {
    for (int i=0; i<num_entries; i++) {

        uint16_t dac_value   = dac_start + i*dac_step;

        float dac_millivolts = 1000. * pulse_vref * dac_value / 16383.;
        float pulse_voltage  = dac_millivolts * shaping_scale_factor;


        float  hi_amplitude = pulse_voltage * attenuation_high;
        float med_amplitude = pulse_voltage * attenuation_med;
        float low_amplitude = pulse_voltage * attenuation_low;

        if (test==test_thresh) {
            amplitude[i] = hi_amplitude;
        }

        else if (test==test_offset) {
            amplitude[i] = (med_amplitude - low_amplitude)/3.5; // 3.5==amp gain
            printf("amplitude=%f (%f-%f)\r\n", amplitude[i], med_amplitude, low_amplitude);
        }

    }
}


float convertIfamp (float voltage) {
        return ( (5.0 - voltage)/r_ifamp);
}

float convertIbias (float voltage) {
        return ( voltage/r_ifamp);
}

float convertIoff (float voltage) {
        return ( (5.0 - voltage)/r_ioff);
}

float convertIamp (float voltage) {
        return ( (5.0 - voltage)/r_iamp);
}

float convert3v3  (float voltage) {
        return ( voltage / (r_ps3v3 * ps3v3_gain_stage_1 * ps3v3_gain_stage_2 ));
}

float convert5v0  (float voltage) {
        return ( voltage / (r_ps5v0 * ps5v0_gain_stage_1 * ps5v0_gain_stage_2 ));
}

// create indexable array of conversion functions
float (*conversion_functions [] ) (float data) = {convertIfamp, convertIamp, convertIoff, convertIbias, convert5v0, convert3v3};

void convertCurrents (float* data, int num_entries, int channel) {

    // need to scale to 1000 or 1000000 depending on uV vs. mV
    float scale_factor = 1;

    if (strcmp(current_units[channel], "mA")==0) {
        scale_factor=1000;
    }
    else if (strcmp(current_units[channel], "uA")==0) {
        scale_factor=1000000;
    }

    // use correct conversion function
    float (*func ) (float data) = (conversion_functions[channel]);

    for (int i=0; i<num_entries; i++) {
        float volts = data[i] * 3.3;
        data[i] = scale_factor *  (*func) (volts);
    }

}
