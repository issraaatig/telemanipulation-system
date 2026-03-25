#include "calibration.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "common_utils.h"

static const char *TAG = "calibration";
static calibration_points_t current_points;

// Angle anchors used for the provided resistance table (x100 => 0.01 deg units).
static const uint16_t k_angle_points_x100[5] = {0, 4500, 7500, 9000, 9000};

// Resistance table from the provided sheet (kOhm), channel order:
// pinky_s1, pinky_s2, ring_s1, ring_s2, middle_s1, middle_s2,
// index_s1, index_s2, thumb_s1, thumb_s2.
static const float k_res_table_kohm[FLEX_SENSOR_COUNT][5] = {
    {152.0f, 143.0f, 141.0f, 140.0f, 140.0f},
    {171.0f, 168.0f, 166.0f, 164.0f, 163.0f},
    {108.0f, 104.0f, 102.0f, 100.0f,  99.0f},
    {144.0f, 140.0f, 139.0f, 137.0f, 135.0f},
    {162.0f, 141.0f, 135.0f, 130.0f, 128.0f},
    {166.0f, 163.91f,163.0f, 162.0f, 161.0f},
    {125.0f, 119.0f, 117.0f, 114.0f, 113.0f},
    {303.0f, 280.0f, 260.0f, 257.0f, 243.0f},
    {185.0f, 177.0f, 175.0f, 173.0f, 172.0f},
    { 93.0f,  88.0f,  87.0f,  87.0f,  87.0f}
};

// Adjust this constant to match your real divider resistor.
// Units are kOhm so output resistance matches the table above.
#define FLEX_FIXED_RESISTOR_KOHM 100.0f
#define ADC_MAX_RAW 4095.0f

static float adc_raw_to_resistance_kohm(uint16_t adc_raw) {
    // Divider model: Vout = Vin * Rs / (Rfixed + Rs) -> Rs = Rfixed * Vout / (Vin - Vout).
    // With ADC ratio, this becomes: Rs = Rfixed * raw / (max_raw - raw).
    float raw = (float)adc_raw;
    if (raw <= 1.0f) {
        return 0.0f;
    }
    if (raw >= (ADC_MAX_RAW - 1.0f)) {
        return 1000000.0f;
    }
    return FLEX_FIXED_RESISTOR_KOHM * (raw / (ADC_MAX_RAW - raw));
}

static uint16_t resistance_to_angle_x100(int channel, float resistance_kohm) {
    const float *r = k_res_table_kohm[channel];

    // Saturate outside table range.
    if (resistance_kohm >= r[0]) {
        return k_angle_points_x100[0];
    }
    if (resistance_kohm <= r[4]) {
        return k_angle_points_x100[4];
    }

    // Piecewise linear interpolation across adjacent calibration anchors.
    for (int i = 0; i < 4; ++i) {
        float r0 = r[i];
        float r1 = r[i + 1];
        if ((resistance_kohm <= r0 && resistance_kohm >= r1) ||
            (resistance_kohm >= r0 && resistance_kohm <= r1)) {
            float angle = linear_interp(
                resistance_kohm,
                r0,
                r1,
                (float)k_angle_points_x100[i],
                (float)k_angle_points_x100[i + 1]);
            if (angle < MIN_ANGLE_X100) {
                return MIN_ANGLE_X100;
            }
            if (angle > MAX_ANGLE_X100) {
                return MAX_ANGLE_X100;
            }
            return (uint16_t)angle;
        }
    }

    return MAX_ANGLE_X100;
}

static void load_defaults(void) {
    for (int i = 0; i < FLEX_SENSOR_COUNT; ++i) {
        current_points.adc_min[i] = 500;
        current_points.adc_max[i] = 3000;
    }
}

esp_err_t calibration_init(void) {
    load_defaults();
    calibration_load(&current_points);
    ESP_LOGI(TAG, "Calibration initialized with resistance-based mapping table");
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
    if (channel < 0 || channel >= FLEX_SENSOR_COUNT) {
        return 0;
    }
    float resistance_kohm = adc_raw_to_resistance_kohm(adc_raw);
    return resistance_to_angle_x100(channel, resistance_kohm);
}
