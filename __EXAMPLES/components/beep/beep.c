#include <stdio.h>
// GPIO
#include "driver/gpio.h"

static const char *TAG = "beep-test";


void beep_setup() {
    // Clean the PIN and setup usage
    gpio_reset_pin(GPIO_NUM_23);
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(GPIO_NUM_23, GPIO_PULLUP_ONLY);
    gpio_set_level(GPIO_NUM_23, 0);
}

void pin_beep(bool led_on_off) {
    // Bip - simple beeper
    if (led_on_off) {
        gpio_set_level(GPIO_NUM_23, 1); // Bip
    } else {
        gpio_set_level(GPIO_NUM_23, 0); // Not Bip
    }
}

// Setup BIP pin
// beep_setup();

// Beep pin
// pin_beep(led_on_off);