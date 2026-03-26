#include "continuous_mapper.h"
#include "pca9685_driver.h"
#include "common_utils.h"
#include "common_config.h"
#include "esp_log.h"

static const char *TAG = "continuous_mapper";

// Glove sensor index convention (from calibration table/order):
// 0 pinky_s1, 1 pinky_s2,
// 2 ring_s1,  3 ring_s2,
// 4 middle_s1,5 middle_s2,
// 6 index_s1, 7 index_s2,
// 8 thumb_s1, 9 thumb_s2.
enum glove_sensor_idx_t {
    S_PINKY_1 = 0,
    S_PINKY_2 = 1,
    S_RING_1 = 2,
    S_RING_2 = 3,
    S_MIDDLE_1 = 4,
    S_MIDDLE_2 = 5,
    S_INDEX_1 = 6,
    S_INDEX_2 = 7,
    S_THUMB_1 = 8,
    S_THUMB_2 = 9
};

// Explicit demo mapping: each servo channel selects one glove sensor.
// This replaces implicit fallback behavior and keeps mapping deterministic.
static const uint8_t k_servo_to_sensor_map[SERVO_COUNT] = {
    S_PINKY_1,  // servo 0
    S_PINKY_2,  // servo 1
    S_RING_1,   // servo 2
    S_RING_2,   // servo 3
    S_MIDDLE_1, // servo 4
    S_MIDDLE_2, // servo 5
    S_INDEX_1,  // servo 6
    S_INDEX_2,  // servo 7
    S_THUMB_1,  // servo 8
    S_THUMB_2,  // servo 9
    S_THUMB_1,  // servo 10 (extra palm/thumb assist)
    S_MIDDLE_1  // servo 11 (extra palm coupling)
};

void continuous_mapper_init(void) {
    ESP_LOGI(TAG,
             "Mapping active: 0..9=direct sensors, 10->thumb_s1, 11->middle_s1");
}

void update_servos(const glove_angles_t *angles) {
    if (!angles) return;
    for (int i = 0; i < SERVO_COUNT; ++i) {
        uint8_t src = k_servo_to_sensor_map[i];
        uint16_t angle = angles->angles[src];
        if (angle < MIN_ANGLE_X100) {
            angle = MIN_ANGLE_X100;
        }
        if (angle > MAX_ANGLE_X100) {
            angle = MAX_ANGLE_X100;
        }
        esp_err_t err = pca9685_set_angle((uint8_t)i, angle);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Servo update failed ch=%d err=%s", i, esp_err_to_name(err));
        }
    }
}
