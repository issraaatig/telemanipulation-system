#include "robot_app.h"
#include "espnow_rx.h"
#include "continuous_mapper.h"
#include "safety_manager.h"
#include "fsr_feedback.h"
#include "pca9685_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "robot_app";
static QueueHandle_t angle_queue;

static void servo_task(void *arg) {
    glove_angles_t angles = {0};
    while (1) {
        if (xQueueReceive(angle_queue, &angles, pdMS_TO_TICKS(20))) {
            safety_manager_heartbeat();
            update_servos(&angles);
        }
        fsr_feedback_poll();
    }
}

void robot_app_start(void) {
    angle_queue = xQueueCreate(5, sizeof(glove_angles_t));
    pca9685_init();
    safety_manager_init();
    fsr_feedback_init();
    espnow_rx_init(angle_queue);

    xTaskCreate(servo_task, "servo_task", 4096, NULL, 10, NULL);
    ESP_LOGI(TAG, "Robot app started");
}
