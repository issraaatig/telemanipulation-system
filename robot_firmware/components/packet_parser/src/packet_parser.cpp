#include "packet_parser.h"

esp_err_t packet_parser_unpack(const uint8_t *buf, int len, glove_angles_t *out_angles) {
    if (!buf || !out_angles || len != CONTROL_PACKET_SIZE_BYTES) {
        return ESP_ERR_INVALID_ARG;
    }
    return control_packet_unpack(buf, out_angles);
}
