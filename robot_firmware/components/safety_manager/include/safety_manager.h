#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t safety_manager_init(void);
void safety_manager_heartbeat(void);
bool safety_manager_motion_allowed(void);

#ifdef __cplusplus
}
#endif
