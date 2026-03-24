#include "continuous_mapper.h"
#include "pca9685_driver.h"
#include "common_utils.h"
#include "common_config.h"

void continuous_mapper_init(void) {
    // No-op for now
}

void update_servos(const glove_angles_t *angles) {
    if (!angles) return;
    for (int i = 0; i < SERVO_COUNT; ++i) {
        int src = i < FLEX_SENSOR_COUNT ? i : FLEX_SENSOR_COUNT - 1;
        pca9685_set_angle((uint8_t)i, angles->angles[src]);
    }
}
