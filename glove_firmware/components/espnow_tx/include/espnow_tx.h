#pragma once

#include "esp_err.h"
#include "control_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t espnow_tx_init(void);
esp_err_t espnow_tx_send(const glove_angles_t *angles);

#ifdef __cplusplus
}
#endif
