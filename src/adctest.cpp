#include "adc.hpp"
#include  <cstdio>

int main ()
{
    Serial::open();
    ADC::init();
    for (int i=0; i<11; i++) {

        //printf("%04X (", ADC::read(i));
        printf("%f\n", ADC::readVoltage(i));
    }
    Serial::close();
}
