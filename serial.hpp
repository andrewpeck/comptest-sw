#define CMD_READ  0xAU
#define CMD_WRITE 0x5U

class Serial {
public:
    Serial();
    ~Serial();

    void Write (unsigned uint8_t adr, unsigned uint32_t write_data)
    uint32_t int Read  (unsigned uint8_t adr)

private:
    int ft_write (char *write_data);
    int ft_read(char* read_data);

    struct ftdi_context *ftdi;
    int f = 0;
    const int vid = 0x0403;
    const int pid = 0x6010;
    const int WRITE_CMD = 0x5;
    const int READ_CMD  = 0xA;
    const int baudrate = 115200;
    int interface = INTERFACE_ANY;
}
