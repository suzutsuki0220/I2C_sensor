#ifndef __BME280_CALIBRATION_H__
#define __BME280_CALIBRATION_H__

#include <stdint.h>
#include "i2c_access.h"

typedef struct calibration {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    int8_t  dig_H1;
    int16_t dig_H2;
    int8_t  dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t  dig_H6;
    signed long int t_fine;
} calibration_t;

void getCalibration(i2c_access *i2c, calibration_t *c);
signed   long int calibration_T(signed long int adc_T, calibration_t *c);
unsigned long int calibration_P(signed long int adc_P, calibration_t *c);
unsigned long int calibration_H(signed long int adc_H, calibration_t *c);

#endif  // __BME280_CALIBRATION_H__
