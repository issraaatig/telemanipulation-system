#pragma once

#include "esp_err.h"
#include "control_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t packet_parser_unpack(const uint8_t *buf, int len, glove_angles_t *out_angles);

#ifdef __cplusplus
}
#endif
