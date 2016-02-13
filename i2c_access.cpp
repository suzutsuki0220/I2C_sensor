#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>

#include "i2c_access.h"

__s32 i2c_smbus_access(int file, char read_write, __u8 command,
               int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;
    __s32 err;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;

    err = ioctl(file, I2C_SMBUS, &args);
    if (err == -1)
        err = -errno;
    return err;
}

__s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
    union i2c_smbus_data data;
    int err;

    err = i2c_smbus_access(file, I2C_SMBUS_READ, command,
                   I2C_SMBUS_BYTE_DATA, &data);
    if (err < 0)
        return err;

    return 0x0FF & data.byte;
}

__s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value)
{
    union i2c_smbus_data data;
    data.byte = value;
    return i2c_smbus_access(file, I2C_SMBUS_WRITE, command,
                I2C_SMBUS_BYTE_DATA, &data);
}

i2c_access::i2c_access()
{
    fd = -1;
    chip_address = 0;
}

i2c_access::~i2c_access()
{
    if (fd >= 0) {
        close(fd);
    }
}

void i2c_access::init(const unsigned int slot, const unsigned int chipaddr)
{
    std::stringstream error_msg_ss;
    char dev_path[80] = {0};

    if (chipaddr < 0x03 || chipaddr > 0x77) {
        error_msg_ss << "Chip address out of range (0x03-0x77)";
        throw error_msg_ss.str();
    }

    snprintf(dev_path, sizeof(dev_path), "/dev/i2c-%d", slot);
    dev_path[sizeof(dev_path) - 1] = '\0';
    fd = open(dev_path, O_RDWR);
    if (fd < 0) {
        error_msg_ss << "Could not open " << dev_path << ": " << strerror(errno);
        throw error_msg_ss.str();
    }

    if (ioctl(fd, I2C_SLAVE_FORCE, chipaddr) < 0) {
        char addr_str[64] = {0};
        snprintf(addr_str, sizeof(addr_str), "0x%02x", chipaddr); 
        error_msg_ss << "Could not set address to " << addr_str << ": " << strerror(errno);
        throw error_msg_ss.str();
    }
    chip_address = chipaddr;
}

int i2c_access::read(const unsigned int data_addr)
{
    int ret;
    std::string error_msg;

    if (fd < 0) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    if (data_addr < 0 || data_addr > 0xff) {
        error_msg = "Data address invalid!";
        throw error_msg;
    }

    ret = i2c_smbus_read_byte_data(fd, data_addr);
    if (ret < 0) {
        error_msg = "I2C bus read failed : ";
        error_msg.append(strerror(ret * -1));
        throw error_msg;
    }
    return ret;
}

void i2c_access::write(const unsigned int data_addr, const unsigned int value)
{
    int ret;
    std::string error_msg;

    if (fd < 0) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    if (data_addr < 0 || data_addr > 0xff) {
        error_msg = "Data address invalid!";
        throw error_msg;
    }

    ret = i2c_smbus_write_byte_data(fd, data_addr, value);
    if (ret < 0) {
        error_msg = "I2C bus write failed : ";
        error_msg.append(strerror(ret * -1));
        throw error_msg;
    }
}

