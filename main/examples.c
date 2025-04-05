#include <stdio.h>
// GPIO
#include "driver/gpio.h"

static const char *TAG = "gpio-test";


// 
// Experimental blocks for stuff
// 
void dump_all_gpio() {
    // https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-reference/peripherals/gpio.html
    gpio_dump_io_configuration(stdout, SOC_GPIO_VALID_GPIO_MASK);
}

void gpio_reset() {
    // Testing all IOs at once
    ESP_LOGI(TAG, "RESet all GPIOs");
    gpio_reset_pin(GPIO_NUM_0); // Bip
    gpio_reset_pin(GPIO_NUM_1); // Silent
    gpio_reset_pin(GPIO_NUM_2); // Bip
    gpio_reset_pin(GPIO_NUM_3); // Bip
    gpio_reset_pin(GPIO_NUM_4); // Silent
    gpio_reset_pin(GPIO_NUM_5); // Silent
    gpio_reset_pin(GPIO_NUM_6); // Silent
    gpio_reset_pin(GPIO_NUM_7); // Silent
    // gpio_reset_pin(GPIO_NUM_8); // LED
    gpio_reset_pin(GPIO_NUM_9); // Silent
    gpio_reset_pin(GPIO_NUM_10); // Bip
    gpio_reset_pin(GPIO_NUM_11); // Bip
    gpio_reset_pin(GPIO_NUM_12); // Bip
    gpio_reset_pin(GPIO_NUM_13); // Bip
    // gpio_reset_pin(GPIO_NUM_14); // Not on board
    gpio_reset_pin(GPIO_NUM_15); // Silent
    // gpio_reset_pin(GPIO_NUM_16); // Not on board
    // gpio_reset_pin(GPIO_NUM_17); // Not on board
    gpio_reset_pin(GPIO_NUM_18); // Silent
    gpio_reset_pin(GPIO_NUM_19); // Silent
    gpio_reset_pin(GPIO_NUM_20); // Silent
    gpio_reset_pin(GPIO_NUM_21); // Silent
    gpio_reset_pin(GPIO_NUM_22); // Silent
    gpio_reset_pin(GPIO_NUM_23); // Silent
    // gpio_reset_pin(GPIO_NUM_24); // **RESERVED**
    // gpio_reset_pin(GPIO_NUM_25); // **RESERVED**
    // gpio_reset_pin(GPIO_NUM_26); // **RESERVED**
    // gpio_reset_pin(GPIO_NUM_27); // Not on board
    // gpio_reset_pin(GPIO_NUM_28); // **RESERVED**
    // gpio_reset_pin(GPIO_NUM_29); // **RESERVED**
    // gpio_reset_pin(GPIO_NUM_30); // **RESERVED**
}

void gpio_set_output() {
    ESP_LOGI(TAG, "Set all not reserved GPIO direction to output");
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT); // Bip
    gpio_set_direction(GPIO_NUM_1, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT); // Bip
    gpio_set_direction(GPIO_NUM_3, GPIO_MODE_OUTPUT); // Bip
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_6, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_7, GPIO_MODE_OUTPUT); // Silent
    // gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT); // LED
    gpio_set_direction(GPIO_NUM_9, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_10, GPIO_MODE_OUTPUT); // Bip
    gpio_set_direction(GPIO_NUM_11, GPIO_MODE_OUTPUT); // Bip
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT); // Bip
    gpio_set_direction(GPIO_NUM_13, GPIO_MODE_OUTPUT); // Bip
    // gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT); // Not on board
    gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT); // Silent
    // gpio_set_direction(GPIO_NUM_16, GPIO_MODE_OUTPUT); // Not on board
    // gpio_set_direction(GPIO_NUM_17, GPIO_MODE_OUTPUT); // Not on board
    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_20, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT); // Silent
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT); // Silent
    // gpio_set_direction(GPIO_NUM_24, GPIO_MODE_OUTPUT); // **RESERVED**
    // gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT); // **RESERVED**
    // gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT); // **RESERVED**
    // gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT); // Not on board
    // gpio_set_direction(GPIO_NUM_28, GPIO_MODE_OUTPUT); // **RESERVED**
    // gpio_set_direction(GPIO_NUM_29, GPIO_MODE_OUTPUT); // **RESERVED**
    // gpio_set_direction(GPIO_NUM_30, GPIO_MODE_OUTPUT); // **RESERVED**
}

void gpio_set_high() {
    ESP_LOGI(TAG, "Set all not reserved GPIO to HIGH");
    gpio_set_level(GPIO_NUM_0, 1); // Bip
    gpio_set_level(GPIO_NUM_1, 1); // Silent
    gpio_set_level(GPIO_NUM_2, 1); // Bip
    gpio_set_level(GPIO_NUM_3, 1); // Bip
    gpio_set_level(GPIO_NUM_4, 1); // Silent
    gpio_set_level(GPIO_NUM_5, 1); // Silent
    gpio_set_level(GPIO_NUM_6, 1); // Silent
    gpio_set_level(GPIO_NUM_7, 1); // Silent
    // gpio_set_level(GPIO_NUM_8, 1); // LED
    gpio_set_level(GPIO_NUM_9, 1); // Silent
    gpio_set_level(GPIO_NUM_10, 1); // Bip
    gpio_set_level(GPIO_NUM_11, 1); // Bip
    gpio_set_level(GPIO_NUM_12, 1); // Bip
    gpio_set_level(GPIO_NUM_13, 1); // Bip
    // gpio_set_level(GPIO_NUM_14, 1); // Not on board
    gpio_set_level(GPIO_NUM_15, 1); // Silent
    // gpio_set_level(GPIO_NUM_16, 1); // Not on board
    // gpio_set_level(GPIO_NUM_17, 1); // Not on board
    gpio_set_level(GPIO_NUM_18, 1); // Silent
    gpio_set_level(GPIO_NUM_19, 1); // Silent
    gpio_set_level(GPIO_NUM_20, 1); // Silent
    gpio_set_level(GPIO_NUM_21, 1); // Silent
    gpio_set_level(GPIO_NUM_22, 1); // Silent
    gpio_set_level(GPIO_NUM_23, 1); // Silent
    // gpio_set_level(GPIO_NUM_24, 1); // **RESERVED**
    // gpio_set_level(GPIO_NUM_25, 1); // **RESERVED**
    // gpio_set_level(GPIO_NUM_26, 1); // **RESERVED**
    // gpio_set_level(GPIO_NUM_27, 1); // Not on board
    // gpio_set_level(GPIO_NUM_28, 1); // **RESERVED**
    // gpio_set_level(GPIO_NUM_29, 1); // **RESERVED**
    // gpio_set_level(GPIO_NUM_30, 1); // **RESERVED**
}

// Example redefining GPIO
void gpio_config_pullup() {
    // https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-reference/peripherals/gpio.html#configure-usb-phy-pins-to-gpio
    gpio_config_t usb_phy_conf = {
        .pin_bit_mask = (1ULL << 1) | (1ULL << 0),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&usb_phy_conf); // Bip
}
