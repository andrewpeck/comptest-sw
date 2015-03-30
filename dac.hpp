class DAC {
    public:
        void write (int dac_counts);
        void writeVoltage (float voltage);
        void setCompDAC();
        void setPulseDAC();
    private:
        int m_dac;
        static const float VREF;

        static const int PULSE_DAC = 0;
        static const int COMP_DAC  = 1;

        static const uint32_t en [2];
        static const uint32_t din[2];
        static const uint32_t clk[2];
        static const uint32_t adr[2];
};
