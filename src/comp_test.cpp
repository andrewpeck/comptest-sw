#include <string>
#include <stdexcept>
#include <time.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <cassert>
/*
 * TODO: figure out what the hell to do with COMPIN
 * TODO: figure out what to expect from COMPOUT
 * TODO: pulse timing analysis
 * TODO: amplitude calibration
 */

namespace Menus
{
    void mainMenu();
}

int main (int argc, char** argv)
{
    if (argc==1) {
        Serial::open();
        ADC::init();

        using namespace ComparatorTest;

        initializeLCT();

        struct TestResult_t result = scanChip();

        std::string filename = now();         // returns current date+time as string
        writeLogFile      (filename, result); // Saves binary record of the raw test result
        writeAsciiLogFile (filename, result); // Writes ASCII Log File
        writeAsciiLogFile ("stdout", result); // Prints log file to STDOUT
        Serial::close();
    }
    else {
        Serial::open();
        Menus::mainMenu();
        Serial::close();
    }
}

namespace comp_test {

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
        fprintf(log,"\t%s i3v3:  %5.02f uA\n", isPassed(checkedResult.currents.i3v3).c_str(),  result.currents.i3v3  );
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

    void writeLogFile (std::string filename, struct TestResult_t result)
    {
        filename += ".dat";
        filename = "log/raw/" + filename;
        FILE *log = fopen(filename.c_str(), "wb");
        fwrite (&result, sizeof(struct TestResult_t), 1, log);
        fclose (log);
    }

    // scans all strips and measures bias and supply currents.
    struct TestResult_t scanChip ()
    {
        auto result = testAllStrips();
        result.currents= Comparator::readComparatorCurrents();
        return result;
    }

    // loop over all strips to find thresholds and offsets.
    struct TestResult_t testAllStrips()
    {
        TestResult_t result;

        double thresh_max = -1*std::numeric_limits<double>::max();
        double thresh_min =    std::numeric_limits<double>::max();

        for (int strip=0; strip<16; strip++) {
            //printf("Testing strip %i (left side)\n", strip);
            auto data = testStrip(strip, LEFT);
            thresh_min = (data.thresh < thresh_min) ? data.thresh : thresh_min;
            thresh_max = (data.thresh > thresh_max) ? data.thresh : thresh_max;
            result.thresh_l[strip] = data.thresh;
            result.offset_l [strip] = data.offset;
        }

        for (int strip=0; strip<16; strip++) {
            //printf("Testing strip %i (right side)\n", strip);
            auto data = testStrip(strip, RIGHT);
            result.thresh_r[strip] = data.thresh;
            thresh_min = (data.thresh < thresh_min) ? data.thresh : thresh_min;
            thresh_max = (data.thresh > thresh_max) ? data.thresh : thresh_max;
            result.offset_r [strip] = data.offset;
        }

        result.thresh_delta = thresh_max - thresh_min;
        return result;
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

        passed.thresh_delta = (REF_THRESH_DELTA_LOW  <= result.thresh_delta  ) &&  (result.thresh_delta  <= REF_THRESH_DELTA_HIGH)  ? 1 : 0;

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
        unsigned int errors = 0;
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

} // namespace
