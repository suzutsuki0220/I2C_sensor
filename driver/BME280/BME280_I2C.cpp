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

#include "BME280_I2C.h"

#define CONFIG_REG_ADDR    0xf5
#define CTRL_MEAS_REG_ADDR 0xf4
#define CTRL_HUM_REG_ADDR  0xf2

/*** Calibration codes ***/
signed long int calibration_T(signed long int adc_T);
unsigned long int calibration_P(signed long int adc_P);
unsigned long int calibration_H(signed long int adc_H);
/******/

BME280_I2C::BME280_I2C()
{
    i2c = NULL;
    forcemode_wait_sleep = true;
}

BME280_I2C::~BME280_I2C()
{
    if (i2c) {
        delete i2c;
    }
}

void
BME280_I2C::init(const unsigned int slot, const unsigned int chipaddr)
{
    try {
        i2c = new i2c_access(10);
        i2c->init(slot, chipaddr);
//        i2c = new spi_access();
//        i2c->init("/dev/spidev0.0", 3, 100000, 8);
    } catch(...) {
        throw;
    }

    init_config(&sensor_conf);

    sensor_conf.mode = 1;
    sensor_conf.standby_time = 5;
    sensor_conf.filter_coefficient = 0;
    sensor_conf.temp_oversample  = 3;
    sensor_conf.hum_oversample   = 3;
    sensor_conf.press_oversample = 3;
    sensor_conf.spi_3wire = 1;

    setup();
    getCalibration();
}

void
BME280_I2C::setup()
{
    int ret;
    uint8_t config_reg;
    uint8_t ctrl_meas_reg;
    uint8_t ctrl_hum;

    config_reg = calc_config_register(&sensor_conf);
    ctrl_meas_reg = calc_ctrl_meas_register(&sensor_conf);
    ctrl_hum  = calc_ctrl_hum_register(&sensor_conf);

    ret = i2c->read(CTRL_HUM_REG_ADDR);
    if ((uint8_t)ret != ctrl_hum) {
        i2c->write(CTRL_HUM_REG_ADDR, ctrl_hum);
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
BME280_I2C::getCalibration(void)
{
    uint8_t data[32] = {
        (uint8_t)i2c->read(0x88), (uint8_t)i2c->read(0x89), (uint8_t)i2c->read(0x8a),
        (uint8_t)i2c->read(0x8b), (uint8_t)i2c->read(0x8c), (uint8_t)i2c->read(0x8d),
        (uint8_t)i2c->read(0x8e), (uint8_t)i2c->read(0x8f), (uint8_t)i2c->read(0x90),
        (uint8_t)i2c->read(0x91), (uint8_t)i2c->read(0x92), (uint8_t)i2c->read(0x93),
        (uint8_t)i2c->read(0x94), (uint8_t)i2c->read(0x95), (uint8_t)i2c->read(0x96),
        (uint8_t)i2c->read(0x97), (uint8_t)i2c->read(0x98), (uint8_t)i2c->read(0x99),
        (uint8_t)i2c->read(0x9a), (uint8_t)i2c->read(0x9b), (uint8_t)i2c->read(0x9c),
        (uint8_t)i2c->read(0x9d), (uint8_t)i2c->read(0x9e), (uint8_t)i2c->read(0x9f),
        (uint8_t)i2c->read(0xa1),  /* skip a0 */
        (uint8_t)i2c->read(0xe1), (uint8_t)i2c->read(0xe2), (uint8_t)i2c->read(0xe3),
        (uint8_t)i2c->read(0xe4), (uint8_t)i2c->read(0xe5), (uint8_t)i2c->read(0xe6),
        (uint8_t)i2c->read(0xe7)
    };

    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11]<< 8) | data[10];
    dig_P4 = (data[13]<< 8) | data[12];
    dig_P5 = (data[15]<< 8) | data[14];
    dig_P6 = (data[17]<< 8) | data[16];
    dig_P7 = (data[19]<< 8) | data[18];
    dig_P8 = (data[21]<< 8) | data[20];
    dig_P9 = (data[23]<< 8) | data[22];
    dig_H1 = data[24];
    dig_H2 = (data[26]<< 8) | data[25];
    dig_H3 = data[27];
    dig_H4 = (data[28]<< 4) | (0x0F & data[29]);
    dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F);
    dig_H6 = data[31];        
}

sensor_mode_t
BME280_I2C::getSensorMode(void)
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
BME280_I2C::getTemperature(void)
{
    std::string error_msg;
    signed long int temp_cal, temp_raw;
    uint8_t val_lsb,val_msb, val_xlsb;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    if (forcemode_wait_sleep && is_force_mode(&sensor_conf)) {
        for (int i = 0; i < 3; i++) {
            usleep(500000);
            if (getSensorMode() == BME280_MODE_SLEEP) {
                forcemode_wait_sleep = false;
                break;
            }
        }
    }

    if (sensor_conf.filter_coefficient == FILTER_COEFFICIENT_OFF) {
        val_xlsb = 0;
    } else {
        val_xlsb = (uint8_t)i2c->read(0xfc);
    }
    val_lsb  = (uint8_t)i2c->read(0xfb);
    val_msb  = (uint8_t)i2c->read(0xfa);

    temp_raw = (val_msb << 12) | (val_lsb << 4) | (val_xlsb >> 4);
    temp_cal = calibration_T(temp_raw);

    return (double)temp_cal / 100.0;
}

double
BME280_I2C::getHumidity(void)
{
    std::string error_msg;
    unsigned long int hum_cal, hum_raw;
    uint8_t val_lsb,val_msb;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    if (forcemode_wait_sleep && is_force_mode(&sensor_conf)) {
        for (int i = 0; i < 3; i++) {
            usleep(500000);
            if (getSensorMode() == BME280_MODE_SLEEP) {
                forcemode_wait_sleep = false;
                break;
            }
        }
    }

    val_lsb = (uint8_t)i2c->read(0xfe);
    val_msb = (uint8_t)i2c->read(0xfd);

    hum_raw = (val_msb << 8) | val_lsb;
    hum_cal = calibration_H(hum_raw);

    return (double)hum_cal / 1024.0;
}

double
BME280_I2C::getPressure(void)
{
    std::string error_msg;
    unsigned long int press_cal, press_raw;
    uint8_t val_lsb,val_msb, val_xlsb;

    if (i2c == NULL) {
        error_msg = "member is not initialized";
        throw error_msg;
    }

    if (forcemode_wait_sleep && is_force_mode(&sensor_conf)) {
        for (int i = 0; i < 3; i++) {
            usleep(500000);
            if (getSensorMode() == BME280_MODE_SLEEP) {
                forcemode_wait_sleep = false;
                break;
            }
        }
    }

    if (sensor_conf.filter_coefficient == FILTER_COEFFICIENT_OFF) {
        val_xlsb = 0;
    } else {
        val_xlsb = (uint8_t)i2c->read(0xf9);
    }
    val_lsb  = (uint8_t)i2c->read(0xf8);
    val_msb  = (uint8_t)i2c->read(0xf7);

    press_raw = (val_msb << 12) | (val_lsb << 4) | (val_xlsb >> 4);
    press_cal = calibration_P(press_raw);

    return (double)press_cal / 100.0;
}

signed long int
BME280_I2C::calibration_T(signed long int adc_T)
{
    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T; 
}

unsigned long int
BME280_I2C::calibration_P(signed long int adc_P)
{
    signed long int var1, var2;
    unsigned long int P;
    var1 = (((signed long int)t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0)
    {
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000)
    {
       P = (P << 1) / ((unsigned long int) var1);   
    }
    else
    {
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
    return P;
}

unsigned long int
BME280_I2C::calibration_H(signed long int adc_H)
{
    signed long int v_x1;
    
    v_x1 = (t_fine - ((signed long int)76800));
    v_x1 = (((((adc_H << 14) -(((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) + 
              ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)dig_H6)) >> 10) * 
              (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) * 
              ((signed long int) dig_H2) + 8192) >> 14));
   v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4));
   v_x1 = (v_x1 < 0 ? 0 : v_x1);
   v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
   return (unsigned long int)(v_x1 >> 12);   
}

