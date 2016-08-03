#include "serial.hpp"
#include "dac.hpp"
#include "mux.hpp"
#include "lctcomp.hpp"
#include "sleep.hpp"
#include "ddd.hpp"
#include "test_params.hpp"


int main ()
{
    CDAC       cdac;
    PDAC       pdac;
    static const auto PKTIME = Comparator::PKTIME50;
    static const auto PKMODE = Comparator::PKMODE2;
    Serial::open();
    Comparator::writeBxDelay(15);
    Comparator::writePulseWidth(8);
    Comparator::writePeakMode(PKMODE);
    Comparator::writePeakTime(PKTIME);
    Comparator::writeTriadPersist(0,1);
    Comparator::writeCompinInject(COMPIN_INJECT);
    DDD::setDelay(0);

    Comparator::writeLCTReset(1);
    usleep(1);
    Comparator::writeLCTReset(0);

    //enum PulseLevel_t {HIGH, MED, LOW, OFF};

    struct Mux::MuxConfig_t muxconfig;
    muxconfig.next = static_cast<Mux::PulseLevel_t>(3);
    muxconfig.prev = static_cast<Mux::PulseLevel_t>(3);

    pdac.write((1<<14)-1);
    //cdac.write();
    usleep(10000);

    while (true) {
        //for (int j=0; j<14; j++)
        //    muxconfig.in[j] = static_cast<Mux::PulseLevel_t>(3);

        //for (int i=0; i<4; i++) {
        //        muxconfig.in[12] = static_cast<Mux::PulseLevel_t>(i);
        //    if (i==0) printf("HIGH\n");
        //    if (i==1) printf("MED\n");
        //    if (i==2) printf("LOW\n");
        //    if (i==3) printf("OFF\n");
        //    Mux::writeConfig(muxconfig);
        //    while (!Comparator::isPulserReady());
        //    for (int i=0; i<10000; i++) {
        //        //printf("pulse %i\n", i);
        //        //while (!Comparator::isPulserReady()) {
        //        //    printf("waiting for pulserdy\n");
        //        //    //fflush(stdout);
        //        //}
        //        Comparator::firePulse();
        //    }
        //}

        //for (int j=0; j<16; j++)
        //    muxconfig.in[j] = static_cast<Mux::PulseLevel_t>(3);

        //muxconfig.in[11] = static_cast<Mux::PulseLevel_t>(1);
        //muxconfig.in[12] = static_cast<Mux::PulseLevel_t>(0);
        //muxconfig.in[13] = static_cast<Mux::PulseLevel_t>(2);
        //Mux::writeConfig(muxconfig);
        //while (!Comparator::isPulserReady());
        //for (int i=0; i<10000; i++) {
        //    //printf("pulse %i\n", i);
        //    //while (!Comparator::isPulserReady()) {
        //    //    printf("waiting for pulserdy\n");
        //    //    //fflush(stdout);
        //    //}
        //    Comparator::firePulse();
        //}


        for (int compin_posneg=0; compin_posneg<2; compin_posneg++) {
        Comparator::writeCompinInject(compin_posneg);
        for (int rst_posneg=0; rst_posneg<2; rst_posneg++) {
        Comparator::writeLCTReset (rst_posneg);
        for (int pkmode=0; pkmode<1; pkmode++) {
        Comparator::writePeakMode(static_cast<Comparator::PKmode_t>(pkmode));
        for (int pktime=0; pktime<1; pktime++) {
        Comparator::writePeakTime(static_cast<Comparator::PKtime_t>(pktime));
        //for (int cdacv=0; cdacv<(1<<14); cdacv+=25) {
        for (int voltage=0; voltage<100; voltage++) {
        cdac.writeThreshold(voltage);

        while (!Comparator::isPulserReady());

        for (int i=0; i<15; i++) {
            printf("pulsing strip %02i\r", i+1);
            fflush(stdout);
            for (int j=0; j<16; j++)
                muxconfig.in[j] = static_cast<Mux::PulseLevel_t>(3); // off

            muxconfig.in[i+0] = static_cast<Mux::PulseLevel_t>(2);   // med
            muxconfig.in[i+1] = static_cast<Mux::PulseLevel_t>(0);   // high
            muxconfig.in[i+2] = static_cast<Mux::PulseLevel_t>(1);   // low

            Mux::writeConfig(muxconfig);

            for (int k=0; k<10; k++) {
                while (!Comparator::isPulserReady());
                Comparator::firePulse();
                auto hs = Comparator::readHalfstrips();
                printf("%08X \n", hs);
            }
        }

        } // cdacv
        } // pktime
        } // pkmode
        } // rst_posneg
        } // compin_posneg
    }

    //while (true) {
    //    for (int i=1; i<120; i++) {
    //        DDD::setDelay(i);
    //        while (!Comparator::isPulserReady());
    //        Comparator::firePulse();
    //    }
    //}

    /*
       for (int i=0; i<(2<<13); i+=500) {
       pdac.write(i);
//printf("Setting dac to %i\n", i);

while (!Comparator::isPulserReady());
Comparator::firePulse();
}
for (int i=((2<<13)-1); i>0; i-=500) {
pdac.write(i);
//printf("Setting dac to %i\n", i);

while (!Comparator::isPulserReady());
Comparator::firePulse();
}
*/

Serial::close();
}
