#include <cerrno>

class spi_access {
private:
    int fd;
    int speed, bits_per_word;

public:
    spi_access();
    ~spi_access();
    void init(const char *i_device, const unsigned int i_mode, const unsigned int i_speed, const unsigned int i_bits_per_word);
    int read(const unsigned int data_addr);
    void write(const unsigned int data_addr, const unsigned char value);
    void write(const unsigned int data_addr, const unsigned char *values, size_t length);
};
