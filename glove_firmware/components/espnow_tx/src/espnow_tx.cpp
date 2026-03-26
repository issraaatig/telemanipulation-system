#include "espnow_tx.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "common_config.h"
#include <cstring>

static const char *TAG = "espnow_tx";
static esp_now_peer_info_t peer_info = {};
static uint32_t tx_ok_count = 0;
static uint32_t tx_fail_count = 0;

static void send_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status) {
    (void)tx_info;
    if (status == ESP_NOW_SEND_SUCCESS) {
        tx_ok_count++;
    } else {
        tx_fail_count++;
        ESP_LOGW(TAG, "ESP-NOW send failed (ok=%lu fail=%lu)",
                 (unsigned long)tx_ok_count, (unsigned long)tx_fail_count);
    }
}

esp_err_t espnow_tx_init(void) {
    ESP_ERROR_CHECK(esp_now_init());
    esp_now_register_send_cb(send_cb);
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
    if (!angles) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t buf[CONTROL_PACKET_SIZE_BYTES];
    esp_err_t err = control_packet_pack(angles, buf);
    if (err != ESP_OK) {
        return err;
    }
    err = esp_now_send(peer_info.peer_addr, buf, CONTROL_PACKET_SIZE_BYTES);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_now_send returned %s", esp_err_to_name(err));
    }
    return err;
}
