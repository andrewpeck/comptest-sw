#ifndef COMP_TEST_HPP
#define COMP_TEST_HPP

namespace ComparatorTest {
    struct ScanResult_t
    {
        double thresh;
        double offset;
    };


    struct TestResult_t
    {
        double  thresh_l [16];
        double  thresh_r [16];
        double  offset_l [16];
        double  offset_r [16];
        struct Comparator::Comparator_currents_t currents;

        double thresh_delta;
    };

    double offset (double threshold);
    void timingScan();
    struct TestResult_t scanChip ();
    struct TestResult_t testAllStrips();
    struct ScanResult_t testStrip(int strip, int side);
    void writeLogFile (std::string filename, struct TestResult_t result);
    void writeAsciiLogFile (std::string filename, struct TestResult_t result);
    int countErrors (struct TestResult_t checkedResult);
    struct TestResult_t checkResult (struct TestResult_t result);
    std::string isPassed (bool pass);
    std::string now();
    void initializeLCT();

    static const int LEFT = 0x0;
    static const int RIGHT = 0x1;
}
#endif
