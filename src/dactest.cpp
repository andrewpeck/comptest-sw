#include "serial.hpp"
#include "dac.hpp"
#include "lctcomp.hpp"
#include <unistd.h>
#include "ddd.hpp"
int main ()
{
    CDAC       cdac;
    PDAC       pdac;
    Serial::open();
    Comparator::writeBxDelay(0);
    Comparator::writePulseWidth(2);

    //for (int i=0; i<1000; i+=2) {
    pdac.write((2<<13) - 1);
    while (true) {
        for (int i=1; i<120; i++) {
            DDD::setDelay(i);
            while (!Comparator::isPulserReady());
            Comparator::firePulse();
        }
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
    }
    Serial::close();
}
