#pragma once
#include "esp_littlefs.h"

#ifdef __cplusplus
extern "C" {
#endif

// FS Setup
void fs_setup(void);
// Read file
void fs_read(void);

#ifdef __cplusplus
}
#endif