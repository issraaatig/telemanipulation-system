#pragma once

#include "control_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

void continuous_mapper_init(void);
void update_servos(const glove_angles_t *angles);

#ifdef __cplusplus
}
#endif
