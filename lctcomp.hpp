class lct_comparator {
    public:
        // Read pattern from comparators
        int pattern_read (struct lct_pattern pattern);

        // Check pattern, expect vs. read
        // return number of errors
        int pattern_check (struct lct_pattern expect, struct lct_pattern read);

        int readHalfstripsErrcnt();
        int readCompoutErrcnt();
        int resetHalfstripsErrcnt();
        int resetCompoutErrcnt();

        void setPeakMode (pkmode peakmode);
        void setPeakTime (pktime peaktime);
        void setPulseWidth (int width);
        void setBxDelay (int delay);

        void setPatternExpect (struct lct_pattern expect);

        struct supply_voltages readSupplyVoltages();
        struct supply_currents readSupplyCurrents();
        struct comparator_voltages read_comparator_voltages();
        struct comparator_currents read_comparator_currents();

        struct lctpattern_t {
            uint32_t halfstrips;
            bool compout;
        };

        enum pkmode_t
        {

        };

        enum pktime_t {

        };


        struct comparator_currents_t {
            float ibias;
            float iamp;
            float ifamp;
            float ioff;
            float i3v3;
            float i5v0;
        };

    private:
}
