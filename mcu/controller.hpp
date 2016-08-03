#ifndef COMP_TEST_HPP
#define COMP_TEST_HPP

namespace ComparatorTest {

    struct ScanResult_t
    {
        double thresh;
        double offset;
    };


    void initializeTestBoard();
    void configurePulser (int strip, int side);
    double offset (double threshold);
    void timingScan();
    struct ScanResult_t testStrip(int strip, int side);

    void initializeLCT();

    static const int LEFT  = 0x0;
    static const int RIGHT = 0x1;
}
#endif
