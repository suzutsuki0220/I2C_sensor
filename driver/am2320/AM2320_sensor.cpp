/**
 *  AM2320 temperature humidity and pressure combined library
 *
 *  @author  suzutsuki0220
 *  @date    29-Jul-2019
 *
 *  This Library is for AOSONG AM2320 sensor module
 *
 *  Copyright 2019 suzutsuki0220
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

#define READING_REG_FUNC      0x03
#define WRITE_MULTI_REG_FUNC  0x10
#define SIZE_HUM_AND_TEMP     0x04
#define HUM_HIGH_ADDR  0x00
#define TEMP_HIGH_ADDR 0x02

#define THROW_ERROR_IF_MEMBER_ISNOT_INIT \
if (i2c == NULL) { \
    std::string _emsg; \
    _emsg = "member is not initialized"; \
    throw _emsg; \
}

double
get2block(unsigned char *block, const size_t address)
{
    uint8_t val_lsb, val_msb;

    val_msb = (uint8_t)block[address];
    val_lsb = (uint8_t)block[address + 1];

    return (double)((val_msb << 8) | val_lsb) / 10.0;
}

void
waitMsec(const long msec)
{
    const long milisec = 1000000;
    struct timespec req = {
        0, 
        msec * milisec
    };

    nanosleep(&req, NULL);
}

AM2320_SENSOR::AM2320_SENSOR()
{
    i2c = NULL;
}

AM2320_SENSOR::~AM2320_SENSOR()
{
    if (i2c) {
        delete i2c;
    }
}

void
AM2320_SENSOR::connect(const unsigned int slot, const unsigned int chipaddr)
{
    try {
        i2c = new i2c_access(10);
        i2c->init(slot, chipaddr);
    } catch(...) {
        throw;
    }
}

void
AM2320_SENSOR::wakeupSensor()
{
    i2c->write(0x00, NULL, 0);
    waitMsec(2);  // wait for awake 800usec - 3msec
}

void
AM2320_SENSOR::readRequest()
{
    unsigned char write_value[] = {0x00, SIZE_HUM_AND_TEMP};  // start address and number of registers

    i2c->write(READING_REG_FUNC, write_value, sizeof(write_value) / sizeof(write_value[0]));
    waitMsec(1.5);
}

void
AM2320_SENSOR::setup(void)
{
    wakeupSensor();
}

void
AM2320_SENSOR::getValue(double &temperature, double &humidity)
{
    unsigned char get_value[6];
    unsigned char* function_code = &get_value[0];  // read, write
    unsigned char* number = &get_value[1];  // number of register
    unsigned char* data   = &get_value[2];  // payload

    THROW_ERROR_IF_MEMBER_ISNOT_INIT;
    setup();

    readRequest();
    i2c->read(0x00, get_value, sizeof(get_value) / sizeof(get_value[0]));

    if (*function_code == READING_REG_FUNC && *number == SIZE_HUM_AND_TEMP) {
        temperature = getTemperature(data);
        humidity = getHumidity(data);
    } else {
        throw "sensor data has an unexpected header";
    }
}

double
AM2320_SENSOR::getTemperature(unsigned char *block)
{
    return get2block(block, TEMP_HIGH_ADDR);
}

double
AM2320_SENSOR::getHumidity(unsigned char *block)
{
    return get2block(block, HUM_HIGH_ADDR);
}
