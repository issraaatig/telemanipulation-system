#include "control_packet.h"
#include <string.h>

esp_err_t control_packet_pack(const glove_angles_t *angles, uint8_t *out_buf) {
    if (!angles || !out_buf) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < CONTROL_PACKET_SENSOR_COUNT; ++i) {
        uint16_t val = angles->angles[i];
        out_buf[2 * i] = (uint8_t)((val >> 8) & 0xFF);
        out_buf[2 * i + 1] = (uint8_t)(val & 0xFF);
    }
    return ESP_OK;
}

esp_err_t control_packet_unpack(const uint8_t *buf, glove_angles_t *out_angles) {
    if (!buf || !out_angles) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < CONTROL_PACKET_SENSOR_COUNT; ++i) {
        uint16_t high = buf[2 * i];
        uint16_t low = buf[2 * i + 1];
        out_angles->angles[i] = (uint16_t)((high << 8) | low);
    }
    return ESP_OK;
}
