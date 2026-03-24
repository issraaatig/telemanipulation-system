#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_mac.h"

#ifdef __cplusplus
extern "C" {
#endif

// Physical limits
#define MIN_ANGLE_DEG 0
#define MAX_ANGLE_DEG 90
#define MIN_ANGLE_X100 (MIN_ANGLE_DEG * 100)
#define MAX_ANGLE_X100 (MAX_ANGLE_DEG * 100)
#define PWM_MIN_US 500
#define PWM_MAX_US 2500

// Timing
#define WIRELESS_TIMEOUT_MS 100
#define SAFETY_HOLD_MS 2000
#define WDT_TIMEOUT_SEC 3

// ESP-NOW
#define ESPNOW_CHANNEL 1
#define ESPNOW_PEER_COUNT 1

// Sensor count
#define FLEX_SENSOR_COUNT 10
#define SERVO_COUNT 12

// Glove ADC mapping (GPIOs)
static const int glove_adc1_gpios[] = {32, 33, 34, 35, 36, 37, 38, 39};
static const int glove_adc2_gpios[] = {25, 26};

// Robot pins
#define PCA9685_SDA_GPIO 21
#define PCA9685_SCL_GPIO 22
#define PCA9685_OE_GPIO  23
#define EMERGENCY_STOP_GPIO 19

// ESP-NOW peer MAC placeholders (set to your devices)
#define MAC_ADDR_LEN 6
extern uint8_t glove_mac[MAC_ADDR_LEN];
extern uint8_t robot_mac[MAC_ADDR_LEN];

/**
 * @brief Load runtime MACs if stored; defaults remain placeholders.
 */
esp_err_t common_config_init(void);

#ifdef __cplusplus
}
#endif
