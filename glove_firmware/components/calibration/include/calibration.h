#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "common_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t adc_min[FLEX_SENSOR_COUNT];
    uint16_t adc_max[FLEX_SENSOR_COUNT];
} calibration_points_t;

esp_err_t calibration_init(void);
esp_err_t calibration_save(const calibration_points_t *points);
esp_err_t calibration_load(calibration_points_t *out_points);
uint16_t calibration_adc_to_angle(int channel, uint16_t adc_raw);

#ifdef __cplusplus
}
#endif
