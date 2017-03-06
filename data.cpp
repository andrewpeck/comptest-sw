
void interpretOffsets (float* data, int num_entries, int full_scale) {
    for (int i=0; i<num_entries; i++) {
        //printf("%f\n", data[i]);
        data[i] = 1.0 - (data[i] / full_scale);
    }
    // we read in error counts and we want to normalize that to the number of pulses, producing a sigmoid efficiency curve
}

void interpretAmplitudes (float* amplitude, int num_entries) {

}

void convertCurrents (float* data, int num_entries, int channel) {

}
