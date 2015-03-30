/* int mux 0-17
 * int config 0, 1, 2, 3
 */

class Mux {
    public:
        void halfstripMaptoMux (uint32_t halfstrips);
        void pulseDistripLH (int triad);
        void pulseDistripRH (int triad);

    private:
        int setMuxConfig (int mux, int config);
};
