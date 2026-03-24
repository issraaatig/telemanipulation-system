#pragma once

#include <stdint.h>
#include "common_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SMA_WINDOW 5

uint16_t map_angle_to_pwm(uint16_t angle_deg_times_100);
float linear_interp(float x, float x0, float x1, float y0, float y1);
void sma_reset(void);
uint16_t sma_update(int channel, uint16_t sample);

#ifdef __cplusplus
}
#endif
