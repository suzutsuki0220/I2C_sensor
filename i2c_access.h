#include <cerrno>

class i2c_access {
private:
    int fd;
    unsigned int chip_address;

public:
    i2c_access();
    ~i2c_access();
    void init(const unsigned int slot, const unsigned int chipaddr);
    int read(const unsigned int data_addr);
    void write(const unsigned int data_addr, const unsigned int value);
};
