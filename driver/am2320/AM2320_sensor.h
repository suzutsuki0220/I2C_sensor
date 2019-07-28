#ifndef __AM2320_SENSOR_H__
#define __AM2320_SENSOR_H__

#include <stdint.h>
#include "i2c_access.h"
#include "spi_access.h"

#define CHIP_ADDR 0x5c

class AM2320_SENSOR {
private:
    i2c_access *i2c;

    void setup(void);
    void wakeupSensor(void);
    void readRequest(void);
    double getTemperature(unsigned char *block);
    double getHumidity(unsigned char *block);

public:
    AM2320_SENSOR();
    ~AM2320_SENSOR();

    void connect(const unsigned int slot, const unsigned int chipaddr = CHIP_ADDR);
    void getValue(double &temperature, double &humidity);
};

#endif  // __AM2320_SENSOR_H__
