#include "sensor_reader.h"
#include "esp_adc_cal.h"
#include "driver/adc.h"
#include "esp_log.h"

static const char *TAG = "sensor_reader";
static esp_adc_cal_characteristics_t adc1_chars;
static esp_adc_cal_characteristics_t adc2_chars;

static const adc1_channel_t adc1_channels[] = {
    ADC1_CHANNEL_4, // GPIO32
    ADC1_CHANNEL_5, // GPIO33
    ADC1_CHANNEL_6, // GPIO34
    ADC1_CHANNEL_7, // GPIO35
    ADC1_CHANNEL_0, // GPIO36
    ADC1_CHANNEL_1, // GPIO37
    ADC1_CHANNEL_2, // GPIO38
    ADC1_CHANNEL_3  // GPIO39
};

static const adc2_channel_t adc2_channels[] = {
    ADC2_CHANNEL_8, // GPIO25
    ADC2_CHANNEL_9  // GPIO26
};

esp_err_t sensor_reader_init(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    for (int i = 0; i < 8; ++i) {
        adc1_config_channel_atten(adc1_channels[i], ADC_ATTEN_DB_11);
    }
    for (int i = 0; i < 2; ++i) {
        adc2_config_channel_atten(adc2_channels[i], ADC_ATTEN_DB_11);
    }
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc1_chars);
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc2_chars);
    return ESP_OK;
}

static uint32_t oversample_adc1(adc1_channel_t ch) {
    uint32_t acc = 0;
    for (int i = 0; i < 16; ++i) {
        acc += adc1_get_raw(ch);
    }
    return acc / 16;
}

static uint32_t oversample_adc2(adc2_channel_t ch) {
    int raw = 0;
    uint32_t acc = 0;
    for (int i = 0; i < 16; ++i) {
        adc2_get_raw(ch, ADC_WIDTH_BIT_12, &raw);
        acc += raw;
    }
    return acc / 16;
}

esp_err_t sensor_reader_read_all(uint16_t *raw_adc_vals) {
    if (!raw_adc_vals) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < 8; ++i) {
        raw_adc_vals[i] = (uint16_t)oversample_adc1(adc1_channels[i]);
    }
    for (int i = 0; i < 2; ++i) {
        raw_adc_vals[8 + i] = (uint16_t)oversample_adc2(adc2_channels[i]);
    }
    return ESP_OK;
}
