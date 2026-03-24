#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t filter_pipeline_init(void);
void filter_angles(uint16_t *raw_adc, uint16_t *angles_out);

#ifdef __cplusplus
}
#endif
