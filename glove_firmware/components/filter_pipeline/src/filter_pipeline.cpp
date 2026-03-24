#include "filter_pipeline.h"
#include "common_utils.h"
#include "calibration.h"
#include "common_config.h"

esp_err_t filter_pipeline_init(void) {
    sma_reset();
    return ESP_OK;
}

void filter_angles(uint16_t *raw_adc, uint16_t *angles_out) {
    if (!raw_adc || !angles_out) return;
    for (int i = 0; i < FLEX_SENSOR_COUNT; ++i) {
        uint16_t smoothed = sma_update(i, raw_adc[i]);
        uint16_t angle = calibration_adc_to_angle(i, smoothed);
        if (angle < MIN_ANGLE_X100) angle = MIN_ANGLE_X100;
        if (angle > MAX_ANGLE_X100) angle = MAX_ANGLE_X100;
        angles_out[i] = angle;
    }
}
