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

environment_driver::environment_driver()
{
}

environment_driver::~environment_driver()
{
}

void
environment_driver::init()
{
}

double
environment_driver::getTemperature(void)
{
    return 27.45f;
}

double
environment_driver::getHumidity(void)
{
    return 48.29;
}

double
environment_driver::getPressure(void)
{
    return 1014.52;
}

