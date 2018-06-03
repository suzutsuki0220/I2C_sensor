#define I2CBUS 1
#define SENSOR_ADDR 0x76

#include <cstdio>
#include <string>

#include "environment_driver.h"

void cgi_header(void);

int main(int argc, char *argv[])
{
    environment_driver sensor;

    double humidity    = 0;
    double temperature = 0;
    double pressure    = 0;

    try {
        sensor = environment_driver();
        sensor.init();

        temperature = sensor.getTemperature();
        humidity = sensor.getHumidity();
        pressure = sensor.getPressure();
    } catch(std::bad_alloc ex) {
        printf("Status: 503\n\n");
        printf("Error: allocation failed : %s\n", ex.what());
        return -1;
    } catch(std::string &err_mes) {
        printf("Status: 503\n\n");
        printf("Error: %s\n", err_mes.c_str());
        return -1;
    }

    cgi_header();

    printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    printf("<sensor>\n");
    printf("    <temperature>%.2f</temperature>\n", temperature);
    printf("    <humidity>%.2f</humidity>\n", humidity);
    printf("    <pressure>%.2f</pressure>\n", pressure);
    printf("</sensor>\n");

    return 0;
}

void cgi_header()
{
    printf("Content-Type: application/xml\n");
    printf("Cache-Control: no-cache\n");
    printf("\n");
}
