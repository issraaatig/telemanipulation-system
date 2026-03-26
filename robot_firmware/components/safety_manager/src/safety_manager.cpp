#include "safety_manager.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "pca9685_driver.h"
#include "common_config.h"

static const char *TAG = "safety";
static int64_t last_rx_us = 0;
static esp_timer_handle_t watchdog_timer;

typedef enum {
    SAFETY_STATE_NORMAL = 0,
    SAFETY_STATE_TIMEOUT_HOLD,
    SAFETY_STATE_SAFE_OPEN,
    SAFETY_STATE_ESTOP
} safety_state_t;

static volatile safety_state_t g_state = SAFETY_STATE_NORMAL;

static void set_state(safety_state_t new_state) {
    if (g_state != new_state) {
        ESP_LOGW(TAG, "Safety state changed: %d -> %d", (int)g_state, (int)new_state);
        g_state = new_state;
    }
}

static void emergency_isr(void *arg) {
    (void)arg;
    set_state(SAFETY_STATE_ESTOP);
    pca9685_enable_output(false);
}

static void watchdog_cb(void *arg) {
    (void)arg;
    if (g_state == SAFETY_STATE_ESTOP) {
        pca9685_enable_output(false);
        return;
    }

    int64_t now = esp_timer_get_time();
    int64_t since_ms = (now - last_rx_us) / 1000;
    if (since_ms > SAFETY_HOLD_MS) {
        set_state(SAFETY_STATE_SAFE_OPEN);
        for (int i = 0; i < SERVO_COUNT; ++i) {
            pca9685_set_angle((uint8_t)i, MIN_ANGLE_X100);
        }
        pca9685_enable_output(false);
    } else if (since_ms > WIRELESS_TIMEOUT_MS) {
        set_state(SAFETY_STATE_TIMEOUT_HOLD);
        pca9685_enable_output(false);
    } else {
        set_state(SAFETY_STATE_NORMAL);
        pca9685_enable_output(true);
    }
}

esp_err_t safety_manager_init(void) {
    last_rx_us = esp_timer_get_time();

    gpio_config_t io = {};
    io.pin_bit_mask = 1ULL << EMERGENCY_STOP_GPIO;
    io.mode = GPIO_MODE_INPUT;
    io.pull_up_en = GPIO_PULLUP_ENABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&io);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(static_cast<gpio_num_t>(EMERGENCY_STOP_GPIO), emergency_isr, NULL);

    const esp_timer_create_args_t args = {
        .callback = watchdog_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "safety_timer",
        .skip_unhandled_events = false
    };
    esp_timer_create(&args, &watchdog_timer);
    esp_timer_start_periodic(watchdog_timer, 20 * 1000); // 20 ms
    return ESP_OK;
}

void safety_manager_heartbeat(void) {
    last_rx_us = esp_timer_get_time();
    if (g_state != SAFETY_STATE_ESTOP) {
        set_state(SAFETY_STATE_NORMAL);
        pca9685_enable_output(true);
    }
}

bool safety_manager_motion_allowed(void) {
    return g_state == SAFETY_STATE_NORMAL;
}
