#ifndef COMP_TEST_HPP
#define COMP_TEST_HPP

namespace ComparatorTest {

    struct TestResult_t
    {
        double  thresh_l [16];
        double  thresh_r [16];
        double  offset_l [16];
        double  offset_r [16];
        struct Comparator::Comparator_currents_t currents;

        double thresh_delta;
    };

    struct TestResult_t scanChip ();
    struct TestResult_t testAllStrips();

    void writeLogFile (std::string filename, struct TestResult_t result);
    void writeAsciiLogFile (std::string filename, struct TestResult_t result);
    int countErrors (struct TestResult_t checkedResult);
    struct TestResult_t checkResult (struct TestResult_t result);
    std::string isPassed (bool pass);
    std::string now();

}
#endif
