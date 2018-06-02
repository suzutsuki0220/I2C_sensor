#define I2CBUS 1
#define SENSOR_ADDR 0x76

#include <cstdio>
#include <string>

#include "BME280_I2C.h"

int main(int argc, char *argv[])
{
    BME280_I2C sensor;

    try {
        sensor = BME280_I2C();
        sensor.init(I2CBUS, SENSOR_ADDR);

        printf("humidity:    %f %%\n", sensor.getHumidity());
        printf("temperature: %f DegC\n", sensor.getTemperature());
        printf("pressure:    %f hPa\n", sensor.getPressure());
    } catch(std::bad_alloc ex) {
        fprintf(stderr, "Error: allocation failed : %s\n", ex.what());
        return -1;
    } catch(std::string &err_mes) {
        fprintf(stderr, "Error: %s\n", err_mes.c_str());
        return -1;
    }

    return 0;
}
