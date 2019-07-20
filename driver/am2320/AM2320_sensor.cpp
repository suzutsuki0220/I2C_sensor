/**
 *  AM2320 temperature humidity and pressure combined library
 *
 *  @author  suzutsuki0220
 *  @date    13-Feb-2016
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
#define HUM_HIGH_ADDR    0x00
#define HUM_LOW_ADDR     0x01
#define TEMP_HIGH_ADDR 0x02
#define TEMP_LOW_ADDR  0x03

#define THROW_ERROR_IF_MEMBER_ISNOT_INIT \
if (i2c == NULL) { \
    std::string _emsg; \
    _emsg = "member is not initialized"; \
    throw _emsg; \
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
AM2320_SENSOR::wakeup()
{
    i2c->write(0x00, NULL, 0);
}

void
AM2320_SENSOR::waitForAwake(void)
{
    waitMsec(3);
}

void
AM2320_SENSOR::readRequest()
{
    unsigned char write_value[] = {0x00, 0x04};

    i2c->write(READING_REG_FUNC, write_value, sizeof(write_value) / sizeof(write_value[0]));
    waitMsec(15);
}

void
AM2320_SENSOR::setup(void)
{
    wakeup();
    waitForAwake();
    readRequest();
}

void
AM2320_SENSOR::getValue(double &temperature, double &humidity)
{
    unsigned char get_value[6];

    THROW_ERROR_IF_MEMBER_ISNOT_INIT;
    setup();

    i2c->read(0x00, get_value, sizeof(get_value) / sizeof(get_value[0]));

    temperature = getTemperature(get_value);
    humidity = getHumidity(get_value);
}

double
AM2320_SENSOR::getTemperature(unsigned char *block)
{
    uint8_t val_lsb, val_msb;

    val_msb = (uint8_t)block[TEMP_HIGH_ADDR + 0x02];
    val_lsb = (uint8_t)block[TEMP_LOW_ADDR + 0x02];

    return (double)((val_msb << 8) | val_lsb) / 10.0;
}

double
AM2320_SENSOR::getHumidity(unsigned char *block)
{
    uint8_t val_lsb,val_msb;

    val_msb = (uint8_t)block[HUM_HIGH_ADDR + 0x02];
    val_lsb = (uint8_t)block[HUM_LOW_ADDR + 0x02];

    return (double)((val_msb << 8) | val_lsb) / 10.0;
}
