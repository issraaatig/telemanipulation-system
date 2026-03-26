#include "espnow_rx.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "packet_parser.h"
#include "common_config.h"
#include <cstring>

static const char *TAG = "espnow_rx";
static QueueHandle_t angle_queue;
static uint32_t rx_bad_size_count = 0;
static uint32_t rx_bad_data_count = 0;
static uint32_t rx_drop_count = 0;

static bool angles_plausible(const glove_angles_t *angles) {
    if (!angles) {
        return false;
    }
    for (int i = 0; i < CONTROL_PACKET_SENSOR_COUNT; ++i) {
        if (angles->angles[i] > MAX_ANGLE_X100) {
            return false;
        }
    }
    return true;
}

static void recv_cb(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (!info || !info->src_addr) {
        return;
    }
    if (memcmp(info->src_addr, glove_mac, MAC_ADDR_LEN) != 0) {
        return;
    }
    if (!angle_queue) {
        return;
    }
    if (len != CONTROL_PACKET_SIZE_BYTES) {
        rx_bad_size_count++;
        if ((rx_bad_size_count % 20U) == 1U) {
            ESP_LOGW(TAG, "Dropped packet: invalid size=%d expected=%d (count=%lu)",
                     len, CONTROL_PACKET_SIZE_BYTES, (unsigned long)rx_bad_size_count);
        }
        return;
    }

    glove_angles_t angles;
    if (packet_parser_unpack(data, len, &angles) != ESP_OK || !angles_plausible(&angles)) {
        rx_bad_data_count++;
        if ((rx_bad_data_count % 20U) == 1U) {
            ESP_LOGW(TAG, "Dropped packet: invalid payload (count=%lu)",
                     (unsigned long)rx_bad_data_count);
        }
        return;
    }

    // ESP-NOW callback runs in Wi-Fi task context, not in a hardware ISR.
    if (xQueueSend(angle_queue, &angles, 0) != pdTRUE) {
        rx_drop_count++;
        glove_angles_t dropped = {0};
        xQueueReceive(angle_queue, &dropped, 0);
        xQueueSend(angle_queue, &angles, 0);
        if ((rx_drop_count % 20U) == 1U) {
            ESP_LOGW(TAG, "RX queue full: dropped oldest packet (count=%lu)",
                     (unsigned long)rx_drop_count);
        }
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
