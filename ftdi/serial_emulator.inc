#ifndef SERIAL_EMULATOR_C
#define SERIAL_EMULATOR_C
void write (uint8_t adr, uint32_t write_data) {
    Emulator::write(adr, write_data);
}
uint32_t read  (uint8_t adr) {
    return(Emulator::read(adr));
}

void open () {}
void close () {}
#endif
