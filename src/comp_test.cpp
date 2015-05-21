#include <string>
#include <stdexcept>
#include <time.h>
#include <iostream>
#include <fstream>
#include <limits>

#define NOMINMAX

#include "sleep.hpp"
#include "lctcomp.hpp"
#include "dac.hpp"
#include "mux.hpp"
#include "ddd.hpp"
#include "comp_test.hpp"
#include "test_params.hpp"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD          "\033[1m"
#define ANSI_RESET         "\033[0m"

/* Default DAQMB constructor sets comp_mode=2, comp_timing=1 */
static const auto PKMODE = Comparator::PKMODE2;
static const auto PKTIME = Comparator::PKTIME50;

/*
 * TODO: figure out what the hell to do with COMPIN
 * TODO: output when no threshold found is screwed
 */


//static Comparator comp;
static CDAC       cdac;
static PDAC       pdac;

namespace Menus {
    void mainMenu();
}

int main (int argc, char** argv)
{
    if (argc==1) {
        Serial::open();
        ADC::init();

        using namespace ComparatorTest;

        struct TestResult_t result = scanChip();

        std::string filename = now();         // returns current date+time as string
        writeLogFile (filename, result);      // Saves binary record of the raw test result
        writeAsciiLogFile (filename, result); // Writes ASCII Log File
        writeAsciiLogFile ("stdout", result); // Writes ASCII Log File
        Serial::close();
    }
    else {
        Serial::open();
        Menus::mainMenu();
        Serial::close();
    }
}

namespace ComparatorTest
{
    void initializeTestBoard() {
        DDD::setDelay(DDD_DELAY);
        Comparator::writeBxDelay(BX_DELAY);
        Comparator::writePulseWidth(PULSE_WIDTH);
        initializeLCT();
    }

    struct ScanResult_t testStrip(int strip, int side)
    {
        /* Sanitizer */
        if (side!=LEFT && side!=RIGHT)
            throw std::runtime_error ("Invalid Halfstrip");

        if (strip<0 && strip>15)
            throw std::runtime_error ("Invalid Strip");


        /* Configure Muxes and Write Pattern Expect */
        configurePulser (strip, side);

        Comparator::writeCompinInject(0);
        ScanResult_t result;
        result.thresh = ~0;
        result.offset = ~0;


        Comparator::writeLCTReset(1);
        Comparator::writeLCTReset(0);

        /* Thresholds */
        cdac.write(CDAC_VALUE);
        usleep(10);
        for (int dac_value=1; dac_value<THRESHOLDS_PDAC_MAX; dac_value+=SCAN_GRANULARITY) {
            printf("\tThresholds DAC setting: %i\r", dac_value);
            fflush(stdout);

            pdac.write(dac_value);
            usleep(10);

            Comparator::resetCounters();

            //while (!Comparator::isPulserReady());
            for (int ipulse=0; ipulse < NUM_PULSES; ipulse++) {
                Comparator::firePulse();
            }

            int errors = Comparator::readThresholdsErrcnt();
            if ((double(errors) / NUM_PULSES) < PASS_THRESHOLD) {
                result.thresh = (1000*pdac.voltage(dac_value)*PULSEAMP_SCALE_FACTOR*ATTENUATION_LOW);
                break;
            }
        }

        printf("\n");

        /* Offsets */
        cdac.write(0);
        usleep(10);
        for (int dac_value=1; dac_value<OFFSETS_PDAC_MAX; dac_value+=SCAN_GRANULARITY) {
            printf("\tOffsets DAC setting: %i\r", dac_value);
            fflush(stdout);

            pdac.write(dac_value);
            usleep(10);

            Comparator::resetCounters();

            //while (!Comparator::isPulserReady());
            for (int ipulse=0; ipulse < NUM_PULSES; ipulse++) {
                Comparator::firePulse();
            }

            int errors  = Comparator::readHalfstripsErrcnt();
            //errors     += Comparator::readCompoutErrcnt();

            if ((double(errors) / NUM_PULSES) < PASS_THRESHOLD) {
                /* we want millivolts */
                result.offset = offset(1000*pdac.voltage(dac_value));
                break;
            }

        }
        printf("\n");
        return result;
    }

    void timingScan()
    {
        initializeLCT();
        int errorSpots [16][120];
        pdac.write(THRESHOLDS_PDAC_MAX);

        for (int bx_delay=0; bx_delay<16; bx_delay++) {
            Comparator::writeBxDelay(bx_delay);
            for (int ddd_delay=0; ddd_delay<120; ddd_delay++) {
                DDD::setDelay(ddd_delay);
                Comparator::resetHalfstripsErrcnt();

                //while (!Comparator::isPulserReady());
                for (int ipulse=0; ipulse < 100; ipulse++) {
                    Comparator::firePulse();
                }

                errorSpots [bx_delay][ddd_delay] = Comparator::readHalfstripsErrcnt();
            }
        }

        char yaxis[17] = "    BX DELAY    ";
        char header[] = "     1        10        20        30        40        50        60        70        80        90        100       110       120\n";

        printf("                                                Pulse Injection (DDD) Delay \n");
        printf("%s", header);
        for (int bx_delay=0; bx_delay<16; bx_delay++) {
            printf("%c %02i ", yaxis[bx_delay], bx_delay);
            for (int ddd_delay=0; ddd_delay<120; ddd_delay++) {
                if (errorSpots[bx_delay][ddd_delay])
                    printf("X");
                else
                    printf(" ");
            }
            printf("\n");
        }
    }

    void initializeLCT()
    {
        cdac.write(CDAC_VALUE);

        Comparator::writePeakMode(PKMODE);    // what should this be?
        Comparator::writePeakTime(PKTIME);    // what should this be?
        Comparator::writeTriadPersist(TRIAD_PERSIST,TRIAD_PERSIST1);   // what should this be?
        Comparator::writeCompinInject(0);     // what should this be?
    }

    struct TestResult_t scanChip ()
    {
        auto result = testAllStrips();
        result.currents= Comparator::readComparatorCurrents();
        return result;
    }

    struct TestResult_t testAllStrips()
    {
        TestResult_t result;

        double thresh_max = std::numeric_limits<double>::min();
        double thresh_min = std::numeric_limits<double>::max();

        for (int strip=0; strip<16; strip++) {
            printf("Testing strip %i (left side)\n", strip);
            auto data = testStrip(strip, LEFT);
            thresh_min = (data.thresh < thresh_min) ? data.thresh : thresh_min;
            thresh_max = (data.thresh > thresh_max) ? data.thresh : thresh_max;
            result.thresh_l[strip] = data.thresh;
            result.offset_l [strip] = data.offset;
        }

        for (int strip=0; strip<16; strip++) {
            printf("Testing strip %i (right side)\n", strip);
            auto data = testStrip(strip, RIGHT);
            result.thresh_r[strip] = data.thresh;
            thresh_min = (data.thresh < thresh_min) ? data.thresh : thresh_min;
            thresh_max = (data.thresh > thresh_max) ? data.thresh : thresh_max;
            result.offset_r [strip] = data.offset;
        }

        result.thresh_delta = thresh_max - thresh_min;
        return result;
    }

    double offset (double threshold) {
        double secondary_amplitude = threshold * PULSEAMP_SCALE_FACTOR * ATTENUATION_MID;
        double tertiary_amplitude  = threshold * PULSEAMP_SCALE_FACTOR * ATTENUATION_HIGH;

        double offset=tertiary_amplitude-secondary_amplitude;
        return offset;
    }

    std::string isPassed (bool pass)
    {
        if (pass)
            return "\x1b[32mPASS\x1b[0m";
        else
            return "\033[1m\x1b[31mFAIL\x1b[0m";
    }

    void writeLogFile (std::string filename, struct TestResult_t result)
    {
        filename += ".dat";
        filename = "log/raw/" + filename;
        FILE *log = fopen(filename.c_str(), "wb");
        fwrite (&result, sizeof(struct TestResult_t), 1, log);
        fclose (log);
    }

    struct TestResult_t checkResult (struct TestResult_t result)
    {
        struct TestResult_t passed;

        for (int i=0; i<16; i++) {
            passed.thresh_l [i] = (REF_THRESH_LOW < result.thresh_l [i]) && (result.thresh_l [i] < REF_THRESH_HIGH) ? 1 : 0;
            passed.thresh_r [i] = (REF_THRESH_LOW < result.thresh_r [i]) && (result.thresh_r [i] < REF_THRESH_HIGH) ? 1 : 0;
            passed.offset_l [i] = (REF_OFFSET_LOW < result.offset_l [i]) && (result.offset_l [i] < REF_OFFSET_HIGH) ? 1 : 0;
            passed.offset_r [i] = (REF_OFFSET_LOW < result.offset_r [i]) && (result.offset_r [i] < REF_OFFSET_HIGH) ? 1 : 0;
        }

        passed.thresh_delta = (REF_THRESH_DELTA_LOW  < result.thresh_delta  ) &&  (result.thresh_delta  < REF_THRESH_DELTA_HIGH)  ? 1 : 0;

        passed.currents.ibias   = (REF_IBIAS_LOW < result.currents.ibias ) &&  (result.currents.ibias < REF_IBIAS_HIGH) ? 1 : 0;
        passed.currents.iamp    = (REF_IAMP_LOW  < result.currents.iamp  ) &&  (result.currents.iamp  < REF_IAMP_HIGH)  ? 1 : 0;
        passed.currents.ifamp   = (REF_IFAMP_LOW < result.currents.ifamp ) &&  (result.currents.ifamp < REF_IFAMP_HIGH) ? 1 : 0;
        passed.currents.ioff    = (REF_IOFF_LOW  < result.currents.ioff  ) &&  (result.currents.ioff  < REF_IOFF_HIGH)  ? 1 : 0;
        passed.currents.i3v3    = (REF_I3V3_LOW  < result.currents.i3v3  ) &&  (result.currents.i3v3  < REF_I3V3_HIGH)  ? 1 : 0;
        passed.currents.i5v0    = (REF_I5V0_LOW  < result.currents.i5v0  ) &&  (result.currents.i5v0  < REF_I5V0_HIGH)  ? 1 : 0;

        return (passed);
    }

    int countErrors (struct TestResult_t checkedResult)
    {
        int errors = 0;
        for (int i=0; i<15; i++) {
            if (!checkedResult.thresh_l [i])
                errors += 1;
            if (!checkedResult.thresh_r [i])
                errors += 1;
            if (!checkedResult.offset_l [i])
                errors += 1;
            if (!checkedResult.offset_r [i])
                errors += 1;
        }

        if (!checkedResult.thresh_delta )
            errors += 1;
        if (!checkedResult.currents.ibias )
            errors += 1;
        if (!checkedResult.currents.iamp  )
            errors += 1;
        if (!checkedResult.currents.ifamp )
            errors += 1;
        if (!checkedResult.currents.ioff  )
            errors += 1;
        if (!checkedResult.currents.i3v3  )
            errors += 1;
        if (!checkedResult.currents.i5v0  )
            errors += 1;

        return errors;
    }

    void writeAsciiLogFile (std::string filename, struct TestResult_t result)
    {
        FILE *log = NULL;

        if (filename=="stdout")
            log = stdout;
        else {
            filename  = "log/" + filename + ".log";
            log = fopen(filename.c_str(), "w");
        }

        struct TestResult_t checkedResult = checkResult (result);

        fprintf(log, ANSI_BOLD "Comparator Currents:\n" ANSI_RESET );
        fprintf(log,"\t%s ibias: %5.02f uA\n", isPassed(checkedResult.currents.ibias).c_str(), result.currents.ibias );
        fprintf(log,"\t%s iamp:  %5.02f uA\n", isPassed(checkedResult.currents.iamp).c_str(),  result.currents.iamp  );
        fprintf(log,"\t%s ifamp: %5.02f uA\n", isPassed(checkedResult.currents.ifamp).c_str(), result.currents.ifamp );
        fprintf(log,"\t%s ioff:  %5.02f uA\n", isPassed(checkedResult.currents.ioff).c_str(),  result.currents.ioff  );
        fprintf(log,"\t%s i3v3:  %5.02f mA\n", isPassed(checkedResult.currents.i3v3).c_str(),  result.currents.i3v3  );
        fprintf(log,"\t%s i5v0:  %5.02f mA\n", isPassed(checkedResult.currents.i5v0).c_str(),  result.currents.i5v0  );

        fprintf(log, ANSI_BOLD "\nThresholds:    (left>right)                    (right>left)\n" ANSI_RESET);
        for (int i=0; i<16; i++) {
            fprintf(log,"\t%s Channel %02i: %05.2f mV", isPassed(checkedResult.thresh_l[i]).c_str(), i+1, result.thresh_l[i]);
            fprintf(log,"\t%s Channel %02i: %05.2f mV\n", isPassed(checkedResult.thresh_r[i]).c_str(), i+1, result.thresh_r[i]);
        }

        fprintf(log, ANSI_BOLD "\nThreshold Delta (thresh max-min) \n" ANSI_RESET);
        fprintf(log,"\t%s % 06.4f mV\n", isPassed(checkedResult.thresh_delta).c_str(), result.thresh_delta);

        fprintf(log, ANSI_BOLD "\nOffset:        (left>right)                    (right>left)\n" ANSI_RESET);
        for (int i=0; i<16; i++) {
            fprintf(log,"\t%s Channel %02i: % 07.4f mV", isPassed(checkedResult.offset_l[i]).c_str(), i+1, result.offset_l[i]);
            fprintf(log,"\t%s Channel %02i: % 07.4f mV\n", isPassed(checkedResult.offset_r[i]).c_str(), i+1, result.offset_r[i]);
        }

        int num_errors = countErrors(checkedResult);
        fprintf(log, ANSI_BOLD "\nSummary:\n" ANSI_RESET);
        fprintf(log,"\t%s with %i errors\n\n", isPassed(!num_errors).c_str(), num_errors);
        fclose (log);
    }

    void configurePulser (int strip, int side)
    {
        struct Mux::MuxConfig_t muxconfig;
        Mux::configAllChannelsOff(muxconfig);

        if (side==LEFT)
            Mux::configStripLH(strip, muxconfig);
        else if (side==RIGHT)
            Mux::configStripRH(strip, muxconfig);

        Mux::writeConfig(muxconfig);

        Comparator::LCTpattern_t pat;

        pat.halfstrips = Mux::configToHalfstripMap(muxconfig);
        pat.compout    = Mux::configToCompoutExpect(muxconfig);

        Comparator::writePatternExpect(pat);
        Comparator::writeActiveStrip(strip);

    }

    #ifdef _WIN32
    #include "timeutils.inc"
    #else
    #include <sys/time.h>
    #endif

	std::string now()
    {
        /* Logging */
        char datestr [80];
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        strftime(datestr, sizeof(datestr)-1, "%Y%m%d-%H%M%S", t);

        timeval curTime;
        gettimeofday(&curTime, NULL);
        int millis = curTime.tv_usec / 1000;

        sprintf(datestr, "%s%03i", datestr, millis);

        std::string filename = datestr;
        return filename;
    }
}
