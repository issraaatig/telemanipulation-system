#include "calibration.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "common_utils.h"

static const char *TAG = "calibration";
static calibration_points_t current_points;

static void load_defaults(void) {
    for (int i = 0; i < FLEX_SENSOR_COUNT; ++i) {
        current_points.adc_min[i] = 500;
        current_points.adc_max[i] = 3000;
    }
}

esp_err_t calibration_init(void) {
    load_defaults();
    calibration_load(&current_points);
    return ESP_OK;
}

esp_err_t calibration_save(const calibration_points_t *points) {
    if (!points) return ESP_ERR_INVALID_ARG;
    nvs_handle_t handle;
    esp_err_t err = nvs_open("calib", NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;
    err = nvs_set_blob(handle, "points", points, sizeof(calibration_points_t));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    if (err == ESP_OK) {
        current_points = *points;
    }
    return err;
}

esp_err_t calibration_load(calibration_points_t *out_points) {
    if (!out_points) return ESP_ERR_INVALID_ARG;
    nvs_handle_t handle;
    esp_err_t err = nvs_open("calib", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        *out_points = current_points;
        return ESP_OK;
    }
    size_t required = sizeof(calibration_points_t);
    err = nvs_get_blob(handle, "points", out_points, &required);
    nvs_close(handle);
    if (err == ESP_OK) {
        current_points = *out_points;
    } else {
        *out_points = current_points;
    }
    return ESP_OK;
}

uint16_t calibration_adc_to_angle(int channel, uint16_t adc_raw) {
    if (channel < 0 || channel >= FLEX_SENSOR_COUNT) return 0;
    uint16_t min = current_points.adc_min[channel];
    uint16_t max = current_points.adc_max[channel];
    if (adc_raw <= min) return MIN_ANGLE_X100;
    if (adc_raw >= max) return MAX_ANGLE_X100;
    float angle = linear_interp((float)adc_raw, (float)min, (float)max, (float)MIN_ANGLE_X100, (float)MAX_ANGLE_X100);
    return (uint16_t)angle;
}
