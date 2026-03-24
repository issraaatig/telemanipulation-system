#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t pca9685_init(void);
esp_err_t pca9685_set_pwm(uint8_t channel, uint16_t on, uint16_t off);
esp_err_t pca9685_set_angle(uint8_t channel, uint16_t angle_deg_times_100);
void pca9685_enable_output(bool enable);

#ifdef __cplusplus
}
#endif
