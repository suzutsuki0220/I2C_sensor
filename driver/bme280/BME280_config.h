#ifndef __BME280_CONFIG_H__
#define __BME280_CONFIG_H__

#include <stdint.h>

#define FILTER_COEFFICIENT_OFF 0x0

typedef struct config {
    uint8_t mode;
    uint8_t standby_time;
    uint8_t filter_coefficient;
    uint8_t temp_oversample;
    uint8_t hum_oversample;
    uint8_t press_oversample;
    uint8_t spi_3wire;
} config_t;

void init_config(config_t *conf);
bool is_force_mode(config_t *conf);
uint8_t calc_config_register(config_t *conf);
uint8_t calc_ctrl_meas_register(config_t *conf);
uint8_t calc_ctrl_hum_register(config_t *conf);

#endif  // __BME280_CONFIG_H__
