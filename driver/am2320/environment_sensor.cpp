/**
 *   temperature humidity sensor AM2320 driver
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

#include "AM2320_sensor.h"
#include "../environment_driver.h"

static AM2320_SENSOR *sensor;
double temperature, humidity;

environment_driver::environment_driver()
{
    sensor = NULL;
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

    try {
        sensor = new AM2320_SENSOR();
        sensor->connect(I2CBUS);
        sensor->getValue(temperature, humidity);
    } catch(...) {
        throw;
    }
}

double
environment_driver::getTemperature(void)
{
    return temperature;
}

double
environment_driver::getHumidity(void)
{
    return humidity;
}

double
environment_driver::getPressure(void)
{
    return 0;  // AM2320 does not have pressure sensor
}
