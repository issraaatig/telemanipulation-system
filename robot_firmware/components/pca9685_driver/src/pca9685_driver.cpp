#include "pca9685_driver.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "common_config.h"
#include "common_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <array>

#define I2C_NUM I2C_NUM_0
#define PCA9685_ADDR 0x40
#define PCA9685_MODE1 0x00
#define PCA9685_PRESCALE 0xFE
#define PCA9685_LED0_ON_L 0x06

static const char *TAG = "pca9685";

static esp_err_t write_reg(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    return i2c_master_write_to_device(I2C_NUM, PCA9685_ADDR, buf, sizeof(buf), pdMS_TO_TICKS(100));
}

static esp_err_t write_regs(uint8_t reg, uint8_t *data, size_t len) {
    if (len > 4) {
        return ESP_ERR_INVALID_ARG;
    }
    std::array<uint8_t, 5> buf = {0};
    buf[0] = reg;
    for (size_t i = 0; i < len; ++i) {
        buf[1 + i] = data[i];
    }
    return i2c_master_write_to_device(I2C_NUM, PCA9685_ADDR, buf.data(), len + 1, pdMS_TO_TICKS(100));
}

esp_err_t pca9685_init(void) {
    i2c_config_t cfg = {};
    cfg.mode = I2C_MODE_MASTER;
    cfg.sda_io_num = PCA9685_SDA_GPIO;
    cfg.scl_io_num = PCA9685_SCL_GPIO;
    cfg.sda_pullup_en = GPIO_PULLUP_ENABLE;
    cfg.scl_pullup_en = GPIO_PULLUP_ENABLE;
    cfg.master.clk_speed = 400000;
    i2c_param_config(I2C_NUM, &cfg);
    i2c_driver_install(I2C_NUM, cfg.mode, 0, 0, 0);

    // Reset mode
    write_reg(PCA9685_MODE1, 0x00);
    vTaskDelay(pdMS_TO_TICKS(5));

    // Set prescale for 50 Hz: prescale = round(25MHz/(4096*50)) - 1 = 121
    write_reg(PCA9685_PRESCALE, 121);
    write_reg(PCA9685_MODE1, 0xA1); // auto-increment + restart

    gpio_config_t oe_cfg = {};
    oe_cfg.pin_bit_mask = 1ULL << PCA9685_OE_GPIO;
    oe_cfg.mode = GPIO_MODE_OUTPUT;
    oe_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    oe_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    oe_cfg.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&oe_cfg);
    pca9685_enable_output(true);

    ESP_LOGI(TAG, "PCA9685 initialized");
    return ESP_OK;
}

esp_err_t pca9685_set_pwm(uint8_t channel, uint16_t on, uint16_t off) {
    uint8_t reg = PCA9685_LED0_ON_L + 4 * channel;
    uint8_t data[4] = {
        (uint8_t)(on & 0xFF),
        (uint8_t)((on >> 8) & 0x0F),
        (uint8_t)(off & 0xFF),
        (uint8_t)((off >> 8) & 0x0F)
    };
    return write_regs(reg, data, sizeof(data));
}

esp_err_t pca9685_set_angle(uint8_t channel, uint16_t angle_deg_times_100) {
    uint16_t pulse_us = map_angle_to_pwm(angle_deg_times_100);
    float ticks = ((float)pulse_us / 20000.0f) * 4096.0f;
    uint16_t off_count = (uint16_t)ticks;
    return pca9685_set_pwm(channel, 0, off_count);
}

void pca9685_enable_output(bool enable) {
    gpio_set_level(static_cast<gpio_num_t>(PCA9685_OE_GPIO), enable ? 0 : 1);
}
