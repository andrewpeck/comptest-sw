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
static const auto PKMODE = Comparator::PKMODE0;
static const auto PKTIME = Comparator::PKTIME25;

//static Comparator comp;
static CDAC       cdac;
static PDAC       pdac;

namespace ComparatorTest
{

    // TODO: parameterize this with some configuration that can be accessed from s/w
    void initializeLCT()
    {
        DDD::setDelay(DDD_DELAY);
        Comparator::writeBxDelay(BX_DELAY);
        Comparator::writePulseWidth(PULSE_WIDTH);

        cdac.writeThreshold(50);

        Comparator::writeLCTReset(1);
        usleep(1);
        Comparator::writeLCTReset(0);

        Comparator::writePeakMode(PKMODE);    // what should this be?
        Comparator::writePeakTime(PKTIME);    // what should this be?
        Comparator::writeTriadPersist(TRIAD_PERSIST,TRIAD_PERSIST1);   // what should this be?
        Comparator::writeCompinInject(COMPIN_INJECT);     // what should this be?
    }

    /* loop over dac values for a single strip. stop when we see expected response and save that threshold */
    /* then loop over dac values to find the offset */
    struct ScanResult_t testStrip(int strip, int side)
    {
        assert (side==LEFT || side==RIGHT);
        assert (strip>=0 && strip<=15);

        /* Configure Muxes and Write Pattern Expect */
        configurePulser (strip, side);

        Comparator::writeCompinInject(COMPIN_INJECT);
        ScanResult_t result;
      //result.thresh = std::numeric_limits<double>::max();
      //result.offset = std::numeric_limits<double>::max();
        result.thresh = ~0;
        result.offset = ~0;

        usleep(1);
      //Comparator::writeLCTReset(1);
        usleep(1);
      //Comparator::writeLCTReset(0);

        /* Thresholds */
        cdac.writeThreshold(50);
        usleep(4000);

        while (!Comparator::isPulserReady()) ; // wait for pulser ready

        for (int dac_value=1; dac_value<THRESHOLDS_PDAC_MAX; dac_value+=SCAN_GRANULARITY) {
            printf("\tThresholds DAC setting: %i\r", dac_value);
            fflush(stdout);

            pdac.write(dac_value);
            /* wait some delay for DAC to settle.. subsequent incremental changes can be faster */
            if (dac_value==1) usleep(100000);

            Comparator::resetCounters();

            for (int ipulse=0; ipulse < NUM_PULSES; ipulse++) {
                //usleep(1);
                Comparator::firePulse();
            }

            unsigned int errors = Comparator::readThresholdsErrcnt();
            //printf("errors %i\n", errors);
            if ((double(errors) / NUM_PULSES) < PASS_THRESHOLD) {
                result.thresh = (1000*pdac.voltage(dac_value)*PULSEAMP_SCALE_FACTOR*ATTENUATION_LOW);
                break;
            }
        }

        //printf("\n");

        /* Offsets */
        cdac.writeThreshold(0);
        usleep(4000);
        while (!Comparator::isPulserReady());
        for (int dac_value=1; dac_value<OFFSETS_PDAC_MAX; dac_value+=SCAN_GRANULARITY) {
            printf("\tOffsets DAC setting: %i\r", dac_value);
            fflush(stdout);

            pdac.write(dac_value);
            /* wait some delay for DAC to settle.. subsequent incremental changes can be faster */
            if (dac_value==1) usleep(100000);

            Comparator::resetCounters();

            for (int ipulse=0; ipulse < NUM_PULSES; ipulse++) {
                Comparator::firePulse();
            }

            unsigned int errors  = Comparator::readHalfstripsErrcnt();
            //int errors = Comparator::readThresholdsErrcnt();
            printf("errors: %08x \n",  errors);
            //errors     += Comparator::readCompoutErrcnt();

            if ((double(errors) / NUM_PULSES) < PASS_THRESHOLD) {
                /* we want millivolts */
                result.offset = offset(1000*pdac.voltage(dac_value));
                break;
            }

        }
        //printf("\n");
        return result;
    }

    void timingScan()
    {
        initializeLCT();
        unsigned int errorSpots [16][120];
        pdac.write((1<<14)-1);


        while (!Comparator::isPulserReady());

        /* Configure Muxes and Write Pattern Expect */
        configurePulser (2, RIGHT);

        cdac.writeThreshold(10);
        Comparator::writeLCTReset (0);

        for (int bx_delay=0; bx_delay<16; bx_delay++) {
            Comparator::writeBxDelay(bx_delay);
            for (int ddd_delay=0; ddd_delay<120; ddd_delay++) {
                DDD::setDelay(ddd_delay);
                Comparator::resetHalfstripsErrcnt();

                for (int ipulse=0; ipulse < 100; ipulse++) {
                    Comparator::firePulse();
                }

                auto hs = Comparator::readHalfstrips();
                    printf("%08X =?", hs);
                    printf("%08X\n", Comparator::readPatternExpect().halfstrips);

                errorSpots [bx_delay][ddd_delay] = Comparator::readHalfstripsErrcnt();

                //if (errorSpots [bx_delay][ddd_delay]  < 9) {
                //    printf("%08X", Comparator::readHalfstrips());
                //}
                //printf("%i\n", errorSpots [bx_delay][ddd_delay]);
            }
        }

        char yaxis[17] = "    BX DELAY    ";
        char header[] = "     1       10        20        30        40        50        60        70        80        90       100       110       120\n";

        printf("                                                Pulse Injection (DDD) Delay \n");
        printf("%s", header);
        for (int bx_delay=0; bx_delay<16; bx_delay++) {
            printf("%c %02i ", yaxis[bx_delay], bx_delay);
            for (int ddd_delay=0; ddd_delay<120; ddd_delay++) {
                if (errorSpots[bx_delay][ddd_delay]>9)
                    printf("X");
                else
                    printf(" ");
            }
            printf("\n");
        }
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

    void configurePulser (int strip, int side)
    {
        struct Mux::MuxConfig_t muxconfig;
        Mux::configAllChannelsOff(muxconfig);

        Mux::configStrip(strip, side, muxconfig);
        Mux::writeConfig(muxconfig);

        Comparator::LCTpattern_t pat;

        pat.halfstrips = Mux::configToHalfstripMap(muxconfig);
        pat.compout    = Mux::configToCompoutExpect(muxconfig);

        Comparator::writePatternExpect(pat);
        Comparator::writeActiveStrip(strip);

        /* the multiplexers cause switching noise on the outputs. Ensure a delay here so that switching noise doesn't
         * cause any undesirable problems when we are looking at pulses */
        usleep(1000);
    }

}
