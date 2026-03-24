#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONTROL_PACKET_SENSOR_COUNT 10
#define CONTROL_PACKET_SIZE_BYTES (CONTROL_PACKET_SENSOR_COUNT * sizeof(uint16_t))

/**
 * @brief Angle payload for all glove sensors.
 * Values are in hundredth-degrees (0.01°). Range 0–9000 => 0.00–90.00°.
 */
typedef struct {
    uint16_t angles[CONTROL_PACKET_SENSOR_COUNT];
} glove_angles_t;

/**
 * @brief Pack angles into big-endian byte buffer.
 *
 * @param angles Pointer to input struct.
 * @param out_buf Destination buffer; must be at least CONTROL_PACKET_SIZE_BYTES.
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on null pointers.
 */
esp_err_t control_packet_pack(const glove_angles_t *angles, uint8_t *out_buf);

/**
 * @brief Unpack big-endian buffer into struct.
 *
 * @param buf Incoming buffer of CONTROL_PACKET_SIZE_BYTES.
 * @param out_angles Destination struct.
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG on null pointers.
 */
esp_err_t control_packet_unpack(const uint8_t *buf, glove_angles_t *out_angles);

#ifdef __cplusplus
}
#endif
