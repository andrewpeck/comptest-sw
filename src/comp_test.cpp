#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include "lctcomp.hpp"
#include "dac.hpp"
#include "mux.hpp"
#include "ddd.hpp"

static const int LEFT = 0x0;
static const int RIGHT = 0x1;

// test parameters
static const float pass_threshold = 0.5;
static const int num_pulses = 1000;

static const int CDAC_VALUE = 120;
Comparator comp;
CDAC cdac;
PDAC pdac;
DDD ddd;
Mux mux;

struct TestResult_t
{
    float  thresh_l [16];
    float  thresh_r [16];
    float  offset_l [15];
    float  offset_r [15];
    struct Comparator::Comparator_currents_t currents;
};


float scanThreshold(int strip, int side);
void writeLogFile (std::string filename, struct TestResult_t result);
void printAsciiLogFile (std::string filename);
void writeAsciiLogFile (std::string filename, struct TestResult_t result);
int countErrors (struct TestResult_t checkedResult);
struct TestResult_t checkResult (struct TestResult_t result);
std::string isPassed (bool pass);
std::string now();
struct TestResult_t scanChip ();
void initializeLCT();
void initializeDDD();


int main (int argc, char** argv)
{
    initializeDDD();
    struct TestResult_t result = scanChip();

    std::string filename = now();         // returns current date+time as string
    writeLogFile (filename, result);      // Saves binary record of the raw test result
    writeAsciiLogFile (filename, result); // Writes ASCII Log File
    printAsciiLogFile (filename);         // Prints the log file to the screen... not very efficient.. but OK..
}

void initializeDDD()
{
    struct DDD::ddd_config dddconf;
    dddconf.ch1enable = true;
    dddconf.ch2enable = true;
    dddconf.ch3enable = true;
    dddconf.ch4enable = true;
    dddconf.ch1delay  = 1;
    dddconf.ch2delay  = 1;
    dddconf.ch3delay  = 1;
    dddconf.ch4delay  = 1;

    ddd.setDelay (dddconf);
}

void initializeLCT()
{
    cdac.write(CDAC_VALUE);

    comp.writePeakMode(Comparator::PKMODE0);   // what should this be?
    comp.writePeakTime(Comparator::PKTIME150); // what should this be?
    comp.writeTriadPersist(0,1);   // what should this be?
    comp.writeCompinInject(0);     // what should this be?
}

struct TestResult_t scanChip ()
{
    initializeLCT();

    struct TestResult_t result;
    result.currents = comp.readComparatorCurrents();

    for (int strip=0; strip<16; strip++) {
        result.thresh_l[strip] = scanThreshold(strip, LEFT);
    }

    for (int strip=0; strip<16; strip++) {
        result.thresh_r[strip] = scanThreshold(strip, RIGHT);
    }

    for (int strip=0; strip<15; strip++) {
        result.offset_l [strip] = result.thresh_l[strip+1]-result.thresh_l[strip];
    }

    for (int strip=0; strip<15; strip++) {
        result.offset_r [strip] = result.thresh_r[strip]-result.thresh_r[strip+1];
    }

    result.currents= comp.readComparatorCurrents();

    return result;
}

std::string now()
{
    /* Logging */
    char datestr [80];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(datestr, sizeof(datestr)-1, "%Y%m%d-%H%M%s", t);
    std::string filename = datestr;
    return filename;
}

std::string isPassed (bool pass)
{
    if (pass)
        return "PASS";
    else
        return "FAIL";
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
    struct TestResult_t refLow;
    for (int i=0; i<16; i++) {
        refLow.thresh_l [i]          = 0.015;
        refLow.thresh_r [i]          = 0.015;
    }
    for (int i=0; i<15; i++) {
        refLow.offset_l [i]          = -0.001;
        refLow.offset_r [i]          = -0.001;
    }

    refLow.currents.ibias = 0;
    refLow.currents.iamp  = 0;
    refLow.currents.ifamp = 0;
    refLow.currents.ioff  = 0;
    refLow.currents.i3v3  = 0;
    refLow.currents.i5v0  = 0;

    struct TestResult_t refHigh;
    for (int i=0; i<16; i++) {
        refHigh.thresh_l [i]          = 0.025;
        refHigh.thresh_r [i]          = 0.025;
    }
    for (int i=0; i<15; i++) {
        refHigh.offset_l [i]          = +0.001;
        refHigh.offset_r [i]          = +0.001;
    }

    refHigh.currents.ibias = 1;
    refHigh.currents.iamp  = 1;
    refHigh.currents.ifamp = 1;
    refHigh.currents.ioff  = 1;
    refHigh.currents.i3v3  = 1;
    refHigh.currents.i5v0  = 1;

    struct TestResult_t passed;

    for (int i=0; i<15; i++) {
        passed.thresh_l [i] = (refLow.thresh_l [i] < result.thresh_l [i]) && (result.thresh_l [i] < refHigh.thresh_l[i]) ? 1 : 0;
        passed.thresh_r [i] = (refLow.thresh_r [i] < result.thresh_r [i]) && (result.thresh_r [i] < refHigh.thresh_r[i]) ? 1 : 0;
    }
    for (int i=0; i<14; i++) {
        passed.offset_l [i] = (refLow.offset_l [i] < result.offset_l [i]) && (result.offset_l [i] < refHigh.offset_l[i]) ? 1 : 0;
        passed.offset_r [i] = (refLow.offset_r [i] < result.offset_r [i]) && (result.offset_r [i] < refHigh.offset_r[i]) ? 1 : 0;
    }

    passed.currents.ibias   = (refLow.currents.ibias < result.currents.ibias ) &&  (result.currents.ibias < refHigh.currents.ibias) ? 1 : 0;
    passed.currents.iamp    = (refLow.currents.iamp  < result.currents.iamp  ) &&  (result.currents.iamp  < refHigh.currents.iamp ) ? 1 : 0;
    passed.currents.ifamp   = (refLow.currents.ifamp < result.currents.ifamp ) &&  (result.currents.ifamp < refHigh.currents.ifamp) ? 1 : 0;
    passed.currents.ioff    = (refLow.currents.ioff  < result.currents.ioff  ) &&  (result.currents.ioff  < refHigh.currents.ioff ) ? 1 : 0;
    passed.currents.i3v3    = (refLow.currents.i3v3  < result.currents.i3v3  ) &&  (result.currents.i3v3  < refHigh.currents.i3v3 ) ? 1 : 0;
    passed.currents.i5v0    = (refLow.currents.i5v0  < result.currents.i5v0  ) &&  (result.currents.i5v0  < refHigh.currents.i5v0 ) ? 1 : 0;

    return (passed);
}


int countErrors (struct TestResult_t checkedResult)
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


void writeAsciiLogFile (std::string filename, struct TestResult_t result)
{
    filename += ".log";
    filename = "log/" + filename;
    FILE *log = fopen(filename.c_str(), "w");

    struct TestResult_t checkedResult = checkResult (result);

    fprintf(log,"\nComparator Currents:\n");
    fprintf(log,"%s ibias: %f\n", isPassed(checkedResult.currents.ibias).c_str(), result.currents.ibias );
    fprintf(log,"%s iamp:  %f\n", isPassed(checkedResult.currents.iamp).c_str(),  result.currents.iamp  );
    fprintf(log,"%s ifamp: %f\n", isPassed(checkedResult.currents.ifamp).c_str(), result.currents.ifamp );
    fprintf(log,"%s ioff:  %f\n", isPassed(checkedResult.currents.ioff).c_str(),  result.currents.ioff  );
    fprintf(log,"%s i3v3:  %f\n", isPassed(checkedResult.currents.i3v3).c_str(),  result.currents.i3v3  );
    fprintf(log,"%s i5v0:  %f\n", isPassed(checkedResult.currents.i5v0).c_str(),  result.currents.i5v0  );

    fprintf(log,"\nThreshold (L>R):\n");
    for (int i=0; i<15; i++) {
        fprintf(log,"%s Channel %02i: % 06.4f\n", isPassed(checkedResult.thresh_l[i]).c_str(), i+1, result.thresh_l[i]);
    }

    fprintf(log,"\nThreshold (R>L):\n");
    for (int i=0; i<15; i++) {
        fprintf(log,"%s Channel %02i: % 06.4f\n", isPassed(checkedResult.thresh_r[i]).c_str(), i+1, result.thresh_r[i]);
    }

    fprintf(log,"\nOffset (L>R):\n");
    for (int i=0; i<14; i++) {
        fprintf(log,"%s Channel %02i: % 06.4f\n", isPassed(checkedResult.offset_l[i]).c_str(), i+1, result.offset_l[i]);
    }

    fprintf(log,"\nOffset (R>L):\n");
    for (int i=0; i<14; i++) {
        fprintf(log,"%s Channel %02i: % 06.4f\n", isPassed(checkedResult.offset_r[i]).c_str(), i+1, result.offset_r[i]);
    }

    int num_errors = countErrors(checkedResult);
    fprintf(log,"\nSummary:\n");
    fprintf(log,"%s with %i errors\n", isPassed(!num_errors).c_str(), num_errors);
    fclose (log);
}

void printAsciiLogFile (std::string filename)
{
    filename += ".log";
    filename = "log/" + filename;

    std::ifstream fin(filename);
    std::string buffer;
    while (getline(fin, buffer))
        std::cout << buffer << std::endl;
    fin.close();
}


float scanThreshold(int strip, int side)
{
    comp.writeLCTReset(1);

    if (side!=LEFT && side!=RIGHT)
        throw std::runtime_error ("Invalid Halfstrip");

    if (strip<0 && strip>15)
        throw std::runtime_error ("Invalid Strip");

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

    int dac_min=0;
    int dac_max=200;

    for (int dac_value=dac_min; dac_value<dac_max; dac_value+=2) {
        //printf("Setting pulsedac to %i: ", dac_value);
        //if ((dac_value%1000)==0)
        //printf("Pulse setting: %i\n", dac_value);
        pdac.write(dac_value);

        comp.writeLCTReset(0);

        usleep(1000);

        comp.resetHalfstripsErrcnt();
        comp.resetCompoutErrcnt();

        for (int ipulse=0; ipulse < num_pulses; ipulse++) {
            while (!comp.isPulserReady());
            comp.firePulse();
        }
        int errors = comp.readHalfstripsErrcnt();
        errors += comp.readCompoutErrcnt();

        //printf("num_errors: %i\n", errors);

        if ((float(errors) / num_pulses) < pass_threshold) {
            //printf("found dac_value: %i\n", dac_value);
            //return (dac_value);
            return (pdac.voltage(dac_value));
        }
    }

    printf("No Threshold Found\n");
    return ~0;
}
