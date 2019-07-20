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

/* Returns the number of read bytes */
/* Until kernel 2.6.22, the length is hardcoded to 32 bytes. If you
   ask for less than 32 bytes, your code will only work with kernels
   2.6.23 and later. */
__s32 i2c_smbus_read_i2c_block_data(int file, __u8 command, __u8 length,
                    __u8 *values)
{
    union i2c_smbus_data data;
    int i, err;

    if (length > I2C_SMBUS_BLOCK_MAX)
        length = I2C_SMBUS_BLOCK_MAX;
    data.block[0] = length;

    err = i2c_smbus_access(file, I2C_SMBUS_READ, command,
                   length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN :
                I2C_SMBUS_I2C_BLOCK_DATA, &data);
    if (err < 0)
        return err;

    for (i = 1; i <= data.block[0]; i++)
        values[i-1] = data.block[i];
    return data.block[0];
}

__s32 i2c_smbus_write_i2c_block_data(int file, __u8 command, __u8 length,
                     const __u8 *values)
{
    union i2c_smbus_data data;
    int i;
    if (length > I2C_SMBUS_BLOCK_MAX)
        length = I2C_SMBUS_BLOCK_MAX;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;
    return i2c_smbus_access(file, I2C_SMBUS_WRITE, command,
                I2C_SMBUS_I2C_BLOCK_BROKEN, &data);
}

i2c_access::i2c_access(const unsigned int retry)
{
    fd = -1;
    retry_times = retry;
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

unsigned char i2c_access::read(const unsigned int data_addr)
{
    unsigned char values[1] = {0};

    read(data_addr, values, 1);

    return values[0];
}

unsigned char i2c_access::read(const unsigned int data_addr, const unsigned char *values, size_t length)
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

    for (unsigned int i=0; i<=retry_times; i++) {
        ret = i2c_smbus_read_i2c_block_data(fd, data_addr, (__u8)length, (__u8*)values);
        if (ret >= 0) {
            break;
        } else {
            if (i < retry_times) {
                usleep(10000);
            }
        }
    }
    if (ret < 0) {
        char addr_str[256] = {0};
        snprintf(addr_str, sizeof(addr_str), "data_addr=0x%02x, ", data_addr);
        error_msg = "I2C bus read failed : ";
        error_msg.append(addr_str);
        error_msg.append(strerror(ret * -1));
        throw error_msg;
    }
    return (unsigned char)ret;
}

void i2c_access::write(const unsigned int data_addr, const unsigned char value)
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

    for (unsigned int i=0; i<=retry_times; i++) {
        ret = i2c_smbus_write_byte_data(fd, data_addr, (__u8)value);
        if (ret == 0) {
            break;
        } else {
            if (i < retry_times) {
                usleep(10000);
            }
        }
    }
    if (ret < 0) {
        char addr_str[256] = {0};
        snprintf(addr_str, sizeof(addr_str), "data_addr=0x%02x, value=0x%02x, ", data_addr, value);
        error_msg = "I2C bus write failed : ";
        error_msg.append(addr_str);
        error_msg.append(strerror(ret * -1));
        throw error_msg;
    }
}

void i2c_access::write(const unsigned int data_addr, const unsigned char *values, size_t length)
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

    for (unsigned int i=0; i<=retry_times; i++) {
        ret = i2c_smbus_write_i2c_block_data(fd, data_addr, (__u8)length, (__u8*)values);
        if (ret == 0) {
            break;
        } else {
            if (i < retry_times) {
                usleep(10000);
            }
        }
    }
    if (ret < 0) {
        char addr_str[256] = {0};
        snprintf(addr_str, sizeof(addr_str), "data_addr=0x%02x, length=%d, ", data_addr, length);
        error_msg = "I2C bus write failed : ";
        error_msg.append(addr_str);
        error_msg.append(strerror(ret * -1));
        throw error_msg;
    }
}
