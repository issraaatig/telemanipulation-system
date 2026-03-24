#include "espnow_rx.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "packet_parser.h"
#include "common_config.h"
#include <cstring>

static const char *TAG = "espnow_rx";
static QueueHandle_t angle_queue;

static void recv_cb(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    (void)info;
    if (!angle_queue || len != CONTROL_PACKET_SIZE_BYTES) return;
    glove_angles_t angles;
    if (packet_parser_unpack(data, len, &angles) == ESP_OK) {
        // ESP-NOW callback runs in Wi-Fi task context, not in a hardware ISR.
        xQueueSend(angle_queue, &angles, 0);
    }
}

esp_err_t espnow_rx_init(QueueHandle_t queue) {
    angle_queue = queue;
    ESP_ERROR_CHECK(esp_now_init());
    esp_now_register_recv_cb(recv_cb);
    esp_now_set_pmk((uint8_t *)"telemanipulate123");

    esp_now_peer_info_t peer = {};
    std::memcpy(peer.peer_addr, glove_mac, MAC_ADDR_LEN);
    peer.channel = ESPNOW_CHANNEL;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    esp_err_t err = esp_now_add_peer(&peer);
    if (err != ESP_OK && err != ESP_ERR_ESPNOW_EXIST) {
        ESP_LOGE(TAG, "Failed to add peer: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}
