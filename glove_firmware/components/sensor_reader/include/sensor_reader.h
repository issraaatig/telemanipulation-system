#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "common_config.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t sensor_reader_init(void);
esp_err_t sensor_reader_read_all(uint16_t *raw_adc_vals);

#ifdef __cplusplus
}
#endif
