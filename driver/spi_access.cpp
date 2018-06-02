#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>

#include "spi_access.h"

spi_access::spi_access()
{
    fd = -1;
    speed = 100000;
    bits_per_word = 8;
}

spi_access::~spi_access()
{
    if (fd >= 0) {
        close(fd);
    }
}

void spi_access::init(const char *i_device, const unsigned int i_mode, const unsigned int i_speed, const unsigned int i_bits_per_word)
{
    std::stringstream error_msg_ss;

    fd = open(i_device, O_RDWR);
    if (fd < 0) {
        error_msg_ss << "Could not open " << i_device << ": " << strerror(errno);
        throw error_msg_ss.str();
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, &i_mode) < 0) {
        error_msg_ss << "Could not set write mode to " << i_mode << ": " << strerror(errno);
        throw error_msg_ss.str();
    }
    if (ioctl(fd, SPI_IOC_RD_MODE, &i_mode) < 0) {
        error_msg_ss << "Could not set read mode to " << i_mode << ": " << strerror(errno);
        throw error_msg_ss.str();
    }
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &i_bits_per_word) < 0) {
        error_msg_ss << "Could not set write bits per word to " << i_bits_per_word << ": " << strerror(errno);
        throw error_msg_ss.str();
    }
    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &i_bits_per_word) < 0) {
        error_msg_ss << "Could not set read bits per word to " << i_bits_per_word << ": " << strerror(errno);
        throw error_msg_ss.str();
    }
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &i_speed) < 0) {
        error_msg_ss << "Could not set write speed to " << i_speed << ": " << strerror(errno);
        throw error_msg_ss.str();
    }
    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &i_speed) < 0) {
        error_msg_ss << "Could not set read speed to " << i_speed << ": " << strerror(errno);
        throw error_msg_ss.str();
    }

    speed = i_speed;
    bits_per_word = i_bits_per_word;
}

int spi_access::read(const unsigned int data_addr)
{
    int ret;
    std::string error_msg;
    char tx[8] = {0};
    char rx[8] = {0};
    struct spi_ioc_transfer spi;
    memset(&spi, 0, sizeof(spi_ioc_transfer));

    if (fd < 0) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    tx[0] = (char)data_addr;
    spi.tx_buf = (unsigned long)tx;
    spi.rx_buf = (unsigned long)rx;
    spi.len = 2;
    spi.delay_usecs = 0;
    spi.speed_hz = speed;
    spi.bits_per_word = bits_per_word;
    
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);
    if (ret < 0) {
        char addr_str[256] = {0};
        snprintf(addr_str, sizeof(addr_str), "data_addr=0x%02x, ", data_addr);
        error_msg = "SPI bus read failed : ";
        error_msg.append(addr_str);
        error_msg.append(strerror(ret * -1));
        throw error_msg;
    }

    return rx[1];
}

void spi_access::write(const unsigned int data_addr, const unsigned char value)
{
    int ret;
    std::string error_msg;
    char tx[8] = {0};
    char rx[8] = {0};
    struct spi_ioc_transfer spi;
    memset(&spi, 0, sizeof(spi_ioc_transfer));

    if (fd < 0) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    tx[0] = (char)data_addr;
    tx[1] = value;
    spi.tx_buf = (unsigned long)tx;
    spi.rx_buf = (unsigned long)rx;
    spi.len = 2;
    spi.delay_usecs = 0;
    spi.speed_hz = speed;
    spi.bits_per_word = bits_per_word;

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);
    if (ret < 0) {
        char addr_str[256] = {0};
        snprintf(addr_str, sizeof(addr_str), "data_addr=0x%02x, value=0x%02x, ", data_addr, value);
        error_msg = "SPI bus write failed : ";
        error_msg.append(addr_str);
        error_msg.append(strerror(ret * -1));
        throw error_msg;
    }
}

void spi_access::write(const unsigned int data_addr, const unsigned char *values, size_t length)
{
    int ret;
    std::string error_msg;

    if (fd < 0) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

/* TODO
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
*/
}


