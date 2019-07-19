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
#include "BME280_calibration.h"

void
getCalibration(i2c_access *i2c, calibration_t *c)
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

    c->dig_T1 = (data[1] << 8) | data[0];
    c->dig_T2 = (data[3] << 8) | data[2];
    c->dig_T3 = (data[5] << 8) | data[4];
    c->dig_P1 = (data[7] << 8) | data[6];
    c->dig_P2 = (data[9] << 8) | data[8];
    c->dig_P3 = (data[11]<< 8) | data[10];
    c->dig_P4 = (data[13]<< 8) | data[12];
    c->dig_P5 = (data[15]<< 8) | data[14];
    c->dig_P6 = (data[17]<< 8) | data[16];
    c->dig_P7 = (data[19]<< 8) | data[18];
    c->dig_P8 = (data[21]<< 8) | data[20];
    c->dig_P9 = (data[23]<< 8) | data[22];
    c->dig_H1 = data[24];
    c->dig_H2 = (data[26]<< 8) | data[25];
    c->dig_H3 = data[27];
    c->dig_H4 = (data[28]<< 4) | (0x0F & data[29]);
    c->dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F);
    c->dig_H6 = data[31];
}

signed long int
calibration_T(signed long int adc_T, calibration_t *c)
{
    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int)c->dig_T1<<1))) * ((signed long int)c->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)c->dig_T1)) * ((adc_T>>4) - ((signed long int)c->dig_T1))) >> 12) * ((signed long int)c->dig_T3)) >> 14;

    c->t_fine = var1 + var2;
    T = (c->t_fine * 5 + 128) >> 8;
    return T; 
}

unsigned long int
calibration_P(signed long int adc_P, calibration_t *c)
{
    signed long int var1, var2;
    unsigned long int P;
    var1 = (((signed long int)c->t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)c->dig_P6);
    var2 = var2 + ((var1*((signed long int)c->dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)c->dig_P4)<<16);
    var1 = (((c->dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)c->dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)c->dig_P1))>>15);
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
    var1 = (((signed long int)c->dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)c->dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + c->dig_P7) >> 4));
    return P;
}

unsigned long int
calibration_H(signed long int adc_H, calibration_t *c)
{
    signed long int v_x1;
    
    v_x1 = (c->t_fine - ((signed long int)76800));
    v_x1 = (((((adc_H << 14) -(((signed long int)c->dig_H4) << 20) - (((signed long int)c->dig_H5) * v_x1)) + 
              ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)c->dig_H6)) >> 10) * 
              (((v_x1 * ((signed long int)c->dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) * 
              ((signed long int) c->dig_H2) + 8192) >> 14));
   v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)c->dig_H1)) >> 4));
   v_x1 = (v_x1 < 0 ? 0 : v_x1);
   v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
   return (unsigned long int)(v_x1 >> 12);   
}

