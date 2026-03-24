#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t safety_manager_init(void);
void safety_manager_heartbeat(void);

#ifdef __cplusplus
}
#endif
