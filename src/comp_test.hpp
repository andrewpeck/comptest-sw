#ifndef COMP_TEST_HPP
#define COMP_TEST_HPP


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

static double offset (double threshold);
static struct TestResult_t scanChip ();
static struct TestResult_t testAllStrips();
static struct ScanResult_t testStrip(int strip, int side);
static void writeLogFile (std::string filename, struct TestResult_t result);
static void writeAsciiLogFile (std::string filename, struct TestResult_t result);
static int countErrors (struct TestResult_t checkedResult);
static struct TestResult_t checkResult (struct TestResult_t result);
static std::string isPassed (bool pass);
static std::string now();
static void initializeLCT();

static const int LEFT = 0x0;
static const int RIGHT = 0x1;

#endif
