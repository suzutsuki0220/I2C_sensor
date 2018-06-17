#ifndef __BME280_SENSOR_H__
#define __BME280_SENSOR_H__

#include <stdint.h>
#include "i2c_access.h"
#include "spi_access.h"

#include "BME280_calibration.h"
#include "BME280_config.h"

typedef enum {
    BME280_MODE_SLEEP,
    BME280_MODE_FORCED,
    BME280_MODE_NORMAL,
} sensor_mode_t;

class BME280_SENSOR {
private:
    i2c_access *i2c;
    //spi_access *i2c;

    void setup(void);
    calibration_t calibration;
    config_t sensor_conf;
    bool forcemode_wait_sleep;

public:
    BME280_SENSOR();
    ~BME280_SENSOR();

    void init(const unsigned int slot, const unsigned int chipaddr);
    sensor_mode_t getSensorMode(void);
    double getTemperature(void);
    double getHumidity(void);
    double getPressure(void);
};

#endif  // __BME280_SENSOR_H__
