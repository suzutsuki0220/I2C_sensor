#ifndef __I2C_ACCESS_H__
#define __I2C_ACCESS_H__

#include <cerrno>

class i2c_access {
private:
    int fd;
    unsigned int retry_times;
    unsigned int chip_address;

public:
    i2c_access(const unsigned int retry_times = 0);
    ~i2c_access();
    void init(const unsigned int slot, const unsigned int chipaddr);
    int read(const unsigned int data_addr);
    void write(const unsigned int data_addr, const unsigned char value);
    void write(const unsigned int data_addr, const unsigned char *values, size_t length);
};

#endif  // __I2C_ACCESS_H__
