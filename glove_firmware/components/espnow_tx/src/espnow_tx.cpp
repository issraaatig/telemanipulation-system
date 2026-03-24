#include "espnow_tx.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "common_config.h"
#include <cstring>

static const char *TAG = "espnow_tx";
static esp_now_peer_info_t peer_info = {0};

esp_err_t espnow_tx_init(void) {
    ESP_ERROR_CHECK(esp_now_init());
    esp_now_set_pmk((uint8_t *)"telemanipulate123");

    std::memset(&peer_info, 0, sizeof(peer_info));
    std::memcpy(peer_info.peer_addr, robot_mac, MAC_ADDR_LEN);
    peer_info.channel = ESPNOW_CHANNEL;
    peer_info.ifidx = WIFI_IF_STA;
    peer_info.encrypt = false;
    esp_err_t err = esp_now_add_peer(&peer_info);
    if (err != ESP_OK && err != ESP_ERR_ESPNOW_EXIST) {
        ESP_LOGE(TAG, "Failed to add peer: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

esp_err_t espnow_tx_send(const glove_angles_t *angles) {
    uint8_t buf[CONTROL_PACKET_SIZE_BYTES];
    control_packet_pack(angles, buf);
    return esp_now_send(peer_info.peer_addr, buf, CONTROL_PACKET_SIZE_BYTES);
}
