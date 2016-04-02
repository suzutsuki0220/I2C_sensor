#include "BME280_config.h"

void
init_config(config_t *conf)
{
    conf->mode = 0;
    conf->standby_time = 0; 
    conf->filter_coefficient = 0;
    conf->temp_oversample  = 0;
    conf->hum_oversample   = 0;
    conf->press_oversample = 0;
    conf->spi_3wire = 0;
}

bool
is_force_mode(config_t *conf)
{
    if (conf->mode == 1 || conf->mode == 2) {
        return true;
    }

    return false;
}

uint8_t
calc_config_register(config_t *conf)
{
    uint8_t ret = 0;
    uint8_t t_sb, filter, spi3w_en;

    t_sb   = conf->standby_time << 5 & 0b11100000;
    filter = conf->filter_coefficient << 2 & 0b00011100;
    spi3w_en = conf->spi_3wire & 0b00000011;

    ret |= t_sb | filter | spi3w_en;

    return ret;
}

uint8_t
calc_ctrl_meas_register(config_t *conf)
{
    uint8_t ret = 0;
    uint8_t osrs_t, osrs_p, mode;

    osrs_t = conf->temp_oversample << 5 & 0b11100000;
    osrs_p = conf->press_oversample << 2 & 0b00011100;
    mode   = conf->mode & 0b00000011;

    ret |= osrs_t | osrs_p | mode;

    return ret;
}

uint8_t
calc_ctrl_hum_register(config_t *conf)
{
    uint8_t ret = 0;
    uint8_t osrs_h;

    osrs_h = conf->hum_oversample & 0b00000111;

    ret |= osrs_h;

    return ret;
}
