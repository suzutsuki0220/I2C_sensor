/**
 *   temperature humidity and pressure sensor stub driver
 *
 *  @author  suzutsuki0220
 *  @date    03-Jun-2018
 *
 *  Copyright 2016 suzutsuki0220
 *     https://github.com/suzutsuki0220/
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <string>
#include <unistd.h>  /* usleep */

#include "BME280_sensor.h"
#include "../environment_driver.h"

static BME280_SENSOR *sensor;
static config_t sensor_conf;

environment_driver::environment_driver()
{
    sensor = NULL;
    init_config(&sensor_conf);
}

environment_driver::~environment_driver()
{
    if (sensor) {
      delete sensor;
    }
}

void
environment_driver::init()
{
    const int I2CBUS = 1;
    const int SENSOR_ADDR = 0x76;

    try {
        sensor_conf.mode = 1;
        sensor_conf.standby_time = 5;
        sensor_conf.filter_coefficient = 0;
        sensor_conf.temp_oversample  = 3;
        sensor_conf.hum_oversample   = 3;
        sensor_conf.press_oversample = 3;
        sensor_conf.spi_3wire = 1;

        sensor = new BME280_SENSOR(&sensor_conf);
        sensor->connect(I2CBUS, SENSOR_ADDR);
        if (sensor->getSensorMode() != BME280_MODE_NORMAL) {
            sensor->setup();
        }
    } catch(...) {
        throw;
    }
}

double
environment_driver::getTemperature(void)
{
    return sensor->getTemperature();
}

double
environment_driver::getHumidity(void)
{
    return sensor->getHumidity();
}

double
environment_driver::getPressure(void)
{
    return sensor->getPressure();
}
