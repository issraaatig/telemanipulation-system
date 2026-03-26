#include "common_config.h"
#include "esp_mac.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "common_config";

uint8_t glove_mac[MAC_ADDR_LEN] = {0x24, 0x6F, 0x28, 0x00, 0x00, 0x01};
uint8_t robot_mac[MAC_ADDR_LEN] = {0x24, 0x6F, 0x28, 0x00, 0x00, 0x02};

static bool mac_is_all(const uint8_t *mac, uint8_t value) {
    for (int i = 0; i < MAC_ADDR_LEN; ++i) {
        if (mac[i] != value) {
            return false;
        }
    }
    return true;
}

static esp_err_t validate_config(void) {
    if (MIN_ANGLE_DEG >= MAX_ANGLE_DEG) {
        ESP_LOGE(TAG, "Invalid angle limits: MIN_ANGLE_DEG >= MAX_ANGLE_DEG");
        return ESP_ERR_INVALID_STATE;
    }
    if (PWM_MIN_US >= PWM_MAX_US) {
        ESP_LOGE(TAG, "Invalid PWM limits: PWM_MIN_US >= PWM_MAX_US");
        return ESP_ERR_INVALID_STATE;
    }

    if (mac_is_all(glove_mac, 0x00) || mac_is_all(glove_mac, 0xFF) ||
        mac_is_all(robot_mac, 0x00) || mac_is_all(robot_mac, 0xFF)) {
        ESP_LOGE(TAG, "Invalid MAC configured (all 00 or all FF)");
        return ESP_ERR_INVALID_STATE;
    }
    if (memcmp(glove_mac, robot_mac, MAC_ADDR_LEN) == 0) {
        ESP_LOGE(TAG, "Invalid MAC configuration: glove and robot MAC are identical");
        return ESP_ERR_INVALID_STATE;
    }

    if (PCA9685_SDA_GPIO == PCA9685_SCL_GPIO ||
        PCA9685_SDA_GPIO == PCA9685_OE_GPIO ||
        PCA9685_SDA_GPIO == EMERGENCY_STOP_GPIO ||
        PCA9685_SCL_GPIO == PCA9685_OE_GPIO ||
        PCA9685_SCL_GPIO == EMERGENCY_STOP_GPIO ||
        PCA9685_OE_GPIO == EMERGENCY_STOP_GPIO) {
        ESP_LOGE(TAG, "Invalid GPIO configuration: duplicate robot pin assignments");
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_err_t common_config_init(void) {
    esp_err_t err = validate_config();
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "Config loaded");
    ESP_LOGI(TAG, "Glove MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             glove_mac[0], glove_mac[1], glove_mac[2], glove_mac[3], glove_mac[4], glove_mac[5]);
    ESP_LOGI(TAG, "Robot MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             robot_mac[0], robot_mac[1], robot_mac[2], robot_mac[3], robot_mac[4], robot_mac[5]);
    ESP_LOGI(TAG, "Pins - I2C SDA:%d SCL:%d OE:%d E-STOP:%d",
             PCA9685_SDA_GPIO, PCA9685_SCL_GPIO, PCA9685_OE_GPIO, EMERGENCY_STOP_GPIO);
    ESP_LOGI(TAG, "Limits - Angle:%d..%d deg PWM:%d..%d us",
             MIN_ANGLE_DEG, MAX_ANGLE_DEG, PWM_MIN_US, PWM_MAX_US);

    return ESP_OK;
}
