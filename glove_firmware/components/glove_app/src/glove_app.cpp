#include "glove_app.h"
#include "sensor_reader.h"
#include "filter_pipeline.h"
#include "calibration.h"
#include "espnow_tx.h"
#include "common_utils.h"
#include "control_packet.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "glove_app";

static void glove_task(void *arg) {
    uint16_t raw[FLEX_SENSOR_COUNT] = {0};
    glove_angles_t angles = {0};
    TickType_t last_wake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(20);

    sma_reset();

    while (1) {
        sensor_reader_read_all(raw);
        filter_angles(raw, angles.angles);
        espnow_tx_send(&angles);
        vTaskDelayUntil(&last_wake, period);
    }
}

void glove_app_start(void) {
    calibration_init();
    sensor_reader_init();
    filter_pipeline_init();
    espnow_tx_init();

    xTaskCreate(glove_task, "glove_task", 4096, NULL, 10, NULL);
    ESP_LOGI(TAG, "Glove app started");
}
