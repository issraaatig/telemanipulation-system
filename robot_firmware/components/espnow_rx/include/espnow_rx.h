#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "control_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t espnow_rx_init(QueueHandle_t queue);

#ifdef __cplusplus
}
#endif
