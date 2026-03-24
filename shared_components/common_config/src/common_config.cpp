#include "common_config.h"
#include "esp_mac.h"
#include "esp_log.h"

static const char *TAG = "common_config";

uint8_t glove_mac[MAC_ADDR_LEN] = {0x24, 0x6F, 0x28, 0x00, 0x00, 0x01};
uint8_t robot_mac[MAC_ADDR_LEN] = {0x24, 0x6F, 0x28, 0x00, 0x00, 0x02};

esp_err_t common_config_init(void) {
    ESP_LOGI(TAG, "Using default MAC placeholders. Update common_config if needed.");
    return ESP_OK;
}
