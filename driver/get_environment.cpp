#include <cstdio>
#include <string>

#include "environment_driver.h"

int main(int argc, char *argv[])
{
    environment_driver sensor;

    try {
        sensor = environment_driver();
        sensor.init();

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
