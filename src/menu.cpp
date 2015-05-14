#include <cstdio>
#include <cstdlib>

#include "lctcomp.hpp"
#include "dac.hpp"
#include "mux.hpp"
#include "ddd.hpp"
#include "comp_test.hpp"
#include "test_params.hpp"

void menuADC();
void menuDDD();
void menuDAC();
void menuPulseInject();
void menuPeekPoke ();

namespace Menus {
    void menuPeekPoke ()
    {
        std::system("clear");
        char cmd [80];
        printf("[0] Read\n");
        printf("[1] Write\n");
        int menu = strtol (fgets(cmd,80,stdin),0,10);
        printf("Address: ");
        int adr = strtol (fgets(cmd,80,stdin),0,0);

        if (menu==0) {
            printf("%08X", Serial::read(adr));
            fgets(cmd,80,stdin);
        }

        if (menu==1) {
            printf("\nData: ");
            int data = strtol (fgets(cmd,80,stdin),0,0);
            Serial::write(adr, data);
            fgets(cmd,80,stdin);
        }
    }

    void menuADC()
    {
        std::system("clear");
        char cmd [80];
        printf("Channel: ");
        int channel = strtol (fgets(cmd,80,stdin),0,10);

        printf("%08X\n", ADC::read(channel));
        fgets(cmd,80,stdin);
    }

    void menuDDD()
    {
        std::system("clear");
        char cmd [80];
        printf("Delay: ");
        int delay = strtol (fgets(cmd,80,stdin),0,10);

        DDD::setDelay(delay);
    }

    void menuDAC()
    {
        std::system("clear");
    }

    void injectOnePulse()
    {
        Comparator::writeBxDelay(BX_DELAY);
        Comparator::writePulseWidth(PULSE_WIDTH);
        while (!Comparator::isPulserReady());
        Comparator::firePulse();
        std::system("clear");
    }

    void menuMuxConfig()
    {
        std::system("clear");
    }

    void menuPulseConfig()
    {
        std::system("clear");
    }

    void menuTimingScan()
    {
        ComparatorTest::timingScan();
        char cmd [80];
        fgets(cmd,80,stdin);
        std::system("clear");
    }

    void mainMenu()
    {
        while (true) {
            std::system("clear");
            printf("[ 1] Peek/Poke Arbitrary Registers\n");
            printf("[ 2] Read ADC\n");
            printf("[ 3] Set DDD\n");
            printf("[ 4] Set DAC\n");
            printf("[ 5] Inject One Pulse \n");
            printf("[ 6] Pulse Config \n");
            printf("[ 7] Mux Config \n");
            printf("[ 8] Timing Scan \n");

            char cmd [80];
            int menu = strtol (fgets(cmd,80,stdin),0,10);

            switch (menu) {
                case 1: menuPeekPoke();   break;
                case 2: menuADC();        break;
                case 3: menuDDD();        break;
                case 4: menuDAC();        break;
                case 5: injectOnePulse(); break;
                case 6: menuPulseConfig(); break;
                case 7: menuMuxConfig();  break;
                case 8: menuTimingScan(); break;
                default: break;
            }
        }
    }
}
