#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
// 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// 
#include "esp_log.h"
#include "esp_err.h"
// File system
#include "esp_system.h"
#include "esp_littlefs.h"

// Working examples
#include "access_point_wifi.h"

static const char *TAG = "playground";

// Cannot move FS tools into a separate module yet.
void fs_setup() {
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };
    // Use settings defined above to initialize and mount LittleFS filesystem.
    // Note: esp_vfs_littlefs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        }
        else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
    }
    else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

void fs_read() {
    ESP_LOGI(TAG, "Reading from flashed filesystem example.txt");
    FILE *f = fopen("/littlefs/example.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    fclose(f);
}


void app_main(void)
{
    ESP_LOGI(TAG, "Start Playground ESP");
    
    // Start Wifi AP
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_setup();

    // FS Setup
    fs_setup();
    // Read file
    fs_read();

    // while (1) {}
}