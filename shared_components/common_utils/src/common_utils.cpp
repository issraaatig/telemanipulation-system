#include "common_utils.h"
#include <string.h>

static uint16_t sma_buffer[FLEX_SENSOR_COUNT][SMA_WINDOW];
static uint32_t sma_sum[FLEX_SENSOR_COUNT];
static uint8_t sma_idx[FLEX_SENSOR_COUNT];

uint16_t map_angle_to_pwm(uint16_t angle_deg_times_100) {
    if (angle_deg_times_100 < MIN_ANGLE_X100) {
        angle_deg_times_100 = MIN_ANGLE_X100;
    }
    if (angle_deg_times_100 > MAX_ANGLE_X100) {
        angle_deg_times_100 = MAX_ANGLE_X100;
    }
    float frac = (float)angle_deg_times_100 / (float)(MAX_ANGLE_X100);
    float pwm = PWM_MIN_US + frac * (float)(PWM_MAX_US - PWM_MIN_US);
    if (pwm < PWM_MIN_US) pwm = PWM_MIN_US;
    if (pwm > PWM_MAX_US) pwm = PWM_MAX_US;
    return (uint16_t)pwm;
}

float linear_interp(float x, float x0, float x1, float y0, float y1) {
    if (x1 - x0 == 0.0f) {
        return y0;
    }
    float t = (x - x0) / (x1 - x0);
    return y0 + t * (y1 - y0);
}

void sma_reset(void) {
    memset(sma_buffer, 0, sizeof(sma_buffer));
    memset(sma_sum, 0, sizeof(sma_sum));
    memset(sma_idx, 0, sizeof(sma_idx));
}

uint16_t sma_update(int channel, uint16_t sample) {
    if (channel < 0 || channel >= FLEX_SENSOR_COUNT) {
        return sample;
    }
    uint8_t idx = sma_idx[channel];
    sma_sum[channel] -= sma_buffer[channel][idx];
    sma_buffer[channel][idx] = sample;
    sma_sum[channel] += sample;
    idx++;
    if (idx >= SMA_WINDOW) {
        idx = 0;
    }
    sma_idx[channel] = idx;
    return (uint16_t)(sma_sum[channel] / SMA_WINDOW);
}
