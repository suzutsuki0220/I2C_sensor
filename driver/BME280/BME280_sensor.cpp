/**
 *  BOSCH BME280 temperature humidity and pressure combined library
 *
 *  @author  suzutsuki0220
 *  @date    13-Feb-2016
 *
 *  This Library is for BME280 sensor module
 *
 *  Calibration code from Arduino sample code by Switch science
 *     http://trac.switch-science.com/wiki/BME280
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

#define RESET_REG_ADDR     0xe0
#define CONFIG_REG_ADDR    0xf5
#define CTRL_MEAS_REG_ADDR 0xf4
#define CTRL_HUM_REG_ADDR  0xf2

BME280_SENSOR::BME280_SENSOR(config_t *conf)
{
    i2c = NULL;
    sensor_conf = conf;
}

BME280_SENSOR::~BME280_SENSOR()
{
    if (i2c) {
        delete i2c;
    }
}

void
BME280_SENSOR::connect(const unsigned int slot, const unsigned int chipaddr)
{
    try {
        i2c = new i2c_access(10);
        i2c->init(slot, chipaddr);
//        i2c = new spi_access();
//        i2c->init("/dev/spidev0.0", 3, 100000, 8);
    } catch(...) {
        throw;
    }
    getCalibration(i2c, &calibration);
}

void
BME280_SENSOR::setup()
{
    int ret;
    uint8_t config_reg;
    uint8_t ctrl_meas_reg;
    uint8_t ctrl_hum_reg;

    config_reg = calc_config_register(sensor_conf);
    ctrl_meas_reg = calc_ctrl_meas_register(sensor_conf);
    ctrl_hum_reg  = calc_ctrl_hum_register(sensor_conf);

    ret = i2c->read(CTRL_HUM_REG_ADDR);
    if ((uint8_t)ret != ctrl_hum_reg) {
        i2c->write(CTRL_HUM_REG_ADDR, ctrl_hum_reg);
    }
    ret = i2c->read(CTRL_MEAS_REG_ADDR);
    if ((uint8_t)ret != ctrl_meas_reg) {
        i2c->write(CTRL_MEAS_REG_ADDR, ctrl_meas_reg);
    }
    ret = i2c->read(CONFIG_REG_ADDR);
    if ((uint8_t)ret != config_reg) {
        i2c->write(CONFIG_REG_ADDR, config_reg);
    }
}

void
BME280_SENSOR::waitForSleepOnForcedmode(void)
{
    if (is_force_mode(sensor_conf)) {
        for (int i = 0; i < 3; i++) {
            usleep(500000);
            if (getSensorMode() == BME280_MODE_SLEEP) {
                break;
            }
        }
    }
}

void
BME280_SENSOR::resetSensor(void)
{
    std::string error_msg;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    i2c->write(RESET_REG_ADDR, 0xb6);
}

sensor_mode_t
BME280_SENSOR::getSensorMode(void)
{
    int ret;
    std::string error_msg;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    ret = i2c->read(CTRL_MEAS_REG_ADDR);

    switch((uint8_t)ret & 0b00000011) {
    case 0:
        return BME280_MODE_SLEEP;
    case 1:
    case 2:
        return BME280_MODE_FORCED;
    case 3:
        return BME280_MODE_NORMAL;
    default:
        error_msg = "Invalid mode value";
        throw(error_msg);
    }
}

double
BME280_SENSOR::getTemperature(void)
{
    std::string error_msg;
    signed long int temp_cal, temp_raw;
    uint8_t val_lsb,val_msb, val_xlsb;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    waitForSleepOnForcedmode();

    if (sensor_conf->filter_coefficient == FILTER_COEFFICIENT_OFF) {
        val_xlsb = 0;
    } else {
        val_xlsb = (uint8_t)i2c->read(0xfc);
    }
    val_lsb  = (uint8_t)i2c->read(0xfb);
    val_msb  = (uint8_t)i2c->read(0xfa);

    temp_raw = (val_msb << 12) | (val_lsb << 4) | (val_xlsb >> 4);
    temp_cal = calibration_T(temp_raw, &calibration);

    return (double)temp_cal / 100.0;
}

double
BME280_SENSOR::getHumidity(void)
{
    std::string error_msg;
    unsigned long int hum_cal, hum_raw;
    uint8_t val_lsb,val_msb;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    waitForSleepOnForcedmode();

    val_lsb = (uint8_t)i2c->read(0xfe);
    val_msb = (uint8_t)i2c->read(0xfd);

    hum_raw = (val_msb << 8) | val_lsb;
    hum_cal = calibration_H(hum_raw, &calibration);

    return (double)hum_cal / 1024.0;
}

double
BME280_SENSOR::getPressure(void)
{
    std::string error_msg;
    unsigned long int press_cal, press_raw;
    uint8_t val_lsb,val_msb, val_xlsb;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    waitForSleepOnForcedmode();

    if (sensor_conf->filter_coefficient == FILTER_COEFFICIENT_OFF) {
        val_xlsb = 0;
    } else {
        val_xlsb = (uint8_t)i2c->read(0xf9);
    }
    val_lsb  = (uint8_t)i2c->read(0xf8);
    val_msb  = (uint8_t)i2c->read(0xf7);

    press_raw = (val_msb << 12) | (val_lsb << 4) | (val_xlsb >> 4);
    press_cal = calibration_P(press_raw, &calibration);

    return (double)press_cal / 100.0;
}
