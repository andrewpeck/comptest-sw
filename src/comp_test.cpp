#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <limits>

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

static const Comparator::PKmode_t   PKMODE = Comparator::PKMODE0;
static const Comparator::PKtime_t   PKTIME = Comparator::PKTIME150;

/* TODO: Definition of OFFSET is wrong
 *       need to calculate the difference between left and right amplitudes
 *
 * TODO: figure out what the hell to do with COMPIN
 *
 * TODO: convert bias currents to microamps scale
 *
 * TODO: colored output
 */


static Comparator comp;
static CDAC       cdac;
static PDAC       pdac;
static DDD        ddd;
static Mux        mux;

int main (int argc, char** argv)
{
    ddd.setDelay(DDD_DELAY);
    initializeLCT();
    struct TestResult_t result = scanChip();

    std::string filename = now();         // returns current date+time as string
    writeLogFile (filename, result);      // Saves binary record of the raw test result
    writeAsciiLogFile (filename, result); // Writes ASCII Log File
    writeAsciiLogFile ("stdout", result); // Writes ASCII Log File
}

static void initializeLCT()
{
    cdac.write(CDAC_VALUE);

    comp.writePeakMode(PKMODE);    // what should this be?
    comp.writePeakTime(PKTIME);    // what should this be?
    comp.writeTriadPersist(TRIAD_PERSIST,TRIAD_PERSIST1);   // what should this be?
    comp.writeCompinInject(0);     // what should this be?
}

static struct TestResult_t scanChip ()
{
    struct TestResult_t result;
    result.currents = comp.readComparatorCurrents();

    double thresh_max = std::numeric_limits<double>::min();
    double thresh_min = std::numeric_limits<double>::max();
    double thresh;

    for (int strip=0; strip<16; strip++) {
        thresh = scanThreshold(strip, LEFT);
        thresh_min = (thresh < thresh_min) ? thresh : thresh_min;
        thresh_max = (thresh > thresh_max) ? thresh : thresh_max;
        result.thresh_l[strip] = thresh;
    }

    for (int strip=0; strip<16; strip++) {
        thresh = scanThreshold(strip, RIGHT);
        result.thresh_r[strip] = thresh;
        thresh_min = (thresh < thresh_min) ? thresh : thresh_min;
        thresh_max = (thresh > thresh_max) ? thresh : thresh_max;
    }

    result.thresh_delta = thresh_max - thresh_min;

    for (int strip=0; strip<15; strip++) {
        result.offset_l [strip] = offset(result.thresh_l[strip]);
    }

    for (int strip=0; strip<15; strip++) {
        result.offset_r [strip] = offset(result.thresh_r[strip]);
    }

    result.currents= comp.readComparatorCurrents();

    return result;
}

static std::string now()
{
    /* Logging */
    char datestr [80];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(datestr, sizeof(datestr)-1, "%Y%m%d-%H%M%s", t);
    std::string filename = datestr;
    return filename;
}

static std::string isPassed (bool pass)
{
    if (pass)
        return "\x1b[32mPASS\x1b[0m";
    else
        return "\033[1m\x1b[31mFAIL\x1b[0m";
}

static void writeLogFile (std::string filename, struct TestResult_t result)
{
    filename += ".dat";
    filename = "log/raw/" + filename;
    FILE *log = fopen(filename.c_str(), "wb");
    fwrite (&result, sizeof(struct TestResult_t), 1, log);
    fclose (log);
}

static struct TestResult_t checkResult (struct TestResult_t result)
{

    struct TestResult_t passed;

    for (int i=0; i<15; i++) {
        passed.thresh_l [i] = (REF_THRESH_LOW < result.thresh_l [i]) && (result.thresh_l [i] < REF_THRESH_HIGH) ? 1 : 0;
        passed.thresh_r [i] = (REF_THRESH_LOW < result.thresh_r [i]) && (result.thresh_r [i] < REF_THRESH_HIGH) ? 1 : 0;
    }
    for (int i=0; i<14; i++) {
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


static int countErrors (struct TestResult_t checkedResult)
{
    int errors = 0;
    for (int i=0; i<15; i++) {
        if (!!!checkedResult.thresh_l [i])
            errors += 1;
        if (!!!checkedResult.thresh_r [i])
            errors += 1;
    }
    for (int i=0; i<14; i++) {
        if (!!!checkedResult.offset_l [i])
            errors += 1;
        if (!!!checkedResult.offset_r [i])
            errors += 1;
    }

    if (!!!checkedResult.thresh_delta )
        errors += 1;

    if (!!!checkedResult.currents.ibias )
        errors += 1;
    if (!!!checkedResult.currents.iamp  )
        errors += 1;
    if (!!!checkedResult.currents.ifamp )
        errors += 1;
    if (!!!checkedResult.currents.ioff  )
        errors += 1;
    if (!!!checkedResult.currents.i3v3  )
        errors += 1;
    if (!!!checkedResult.currents.i5v0  )
        errors += 1;

    return errors;
}


static void writeAsciiLogFile (std::string filename, struct TestResult_t result)
{
    FILE *log = NULL;

    if (filename=="stdout")
        log = stdout;
    else {
    filename += ".log";
    filename = "log/" + filename;
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
    for (int i=0; i<15; i++) {
        fprintf(log,"\t%s Channel %02i: %05.2f mV", isPassed(checkedResult.thresh_l[i]).c_str(), i+1, result.thresh_l[i]);
        fprintf(log,"\t%s Channel %02i: %05.2f mV\n", isPassed(checkedResult.thresh_r[i]).c_str(), i+1, result.thresh_r[i]);
    }

    fprintf(log, ANSI_BOLD "\nThreshold Delta (thresh max-min) \n" ANSI_RESET);
    fprintf(log,"\t%s % 06.4f mV\n", isPassed(checkedResult.thresh_delta).c_str(), result.thresh_delta);

    fprintf(log, ANSI_BOLD "\nOffset:        (left>right)                    (right>left)\n" ANSI_RESET);
    for (int i=0; i<14; i++) {
        fprintf(log,"\t%s Channel %02i: % 06.3f mV", isPassed(checkedResult.offset_l[i]).c_str(), i+1, result.offset_l[i]);
        fprintf(log,"\t%s Channel %02i: % 06.3f mV\n", isPassed(checkedResult.offset_r[i]).c_str(), i+1, result.offset_r[i]);
    }

    int num_errors = countErrors(checkedResult);
    fprintf(log, ANSI_BOLD "\nSummary:\n" ANSI_RESET);
    fprintf(log,"\t%s with %i errors\n\n", isPassed(!num_errors).c_str(), num_errors);
    fclose (log);
}

static double scanThreshold(int strip, int side)
{
    /* Sanitizer */
    if (side!=LEFT && side!=RIGHT)
        throw std::runtime_error ("Invalid Halfstrip");

    if (strip<0 && strip>15)
        throw std::runtime_error ("Invalid Strip");


    comp.writeLCTReset(1);

    /* Configure Muxes and Write Pattern Expect */
    Mux::MuxConfig_t muxconfig;
    mux.configAllChannelsOff(muxconfig);

    if (side==LEFT)
        mux.configStripLH(strip, muxconfig);
    else if (side==RIGHT)
        mux.configStripRH(strip, muxconfig);

    mux.writeConfig(muxconfig);

    Comparator::LCTpattern_t pat;
    pat.halfstrips = mux.configToHalfstripMap(muxconfig);
    pat.compout = mux.configToCompoutExpect(muxconfig);
    comp.writePatternExpect(pat);

    /* Should inject the inputs when pulsing strip 0 */
    /* TODO: I don't understand this... */
    if (strip==0)
        comp.writeCompinInject(1);

    for (int dac_value=PDAC_MIN; dac_value<PDAC_MAX; dac_value+=SCAN_GRANULARITY) {
        pdac.write(dac_value);

        comp.writeLCTReset(0);

        usleep(1);

        comp.resetHalfstripsErrcnt();
        comp.resetCompoutErrcnt();

        for (int ipulse=0; ipulse < NUM_PULSES; ipulse++) {
            while (!comp.isPulserReady());
            comp.firePulse();
        }
        int errors = comp.readHalfstripsErrcnt();
        errors    += comp.readCompoutErrcnt();

        if ((double(errors) / NUM_PULSES) < PASS_THRESHOLD) {
            /* we want millivolts */
            return (1000*pdac.voltage(dac_value)*PULSEAMP_SCALE_FACTOR*ATTENUATION_LOW);
        }
    }

    printf("No Threshold Found\n");
    return ~0;
}
