// LED
#include "led_strip.h"

// Do not used
#define LED_STRIP_USE_DMA 0

#define LED_STRIP_LED_COUNT 1
#define LED_STRIP_MEMORY_BLOCK_WORDS 0 // let the driver choose a proper memory block size automatically

#define LED_STRIP_GPIO_PIN 8
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)

static const char *TAG = "blink";

// LED OPTS
#define TIME_TICK_MS 500

// 
// Common blocks that are working and indicative
// 
led_strip_handle_t configure_led(void) {
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN, // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_COUNT,      // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,        // LED strip model
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color order of the strip: GRB
        .flags = {
            .invert_out = false, // don't invert the output signal
        }
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
        .mem_block_symbols = LED_STRIP_MEMORY_BLOCK_WORDS, // the memory block size used by the RMT channel
        .flags = {
            .with_dma = LED_STRIP_USE_DMA,     // Using DMA can improve performance when driving more LEDs
        }
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
    return led_strip;
}

int random_int_range(int min, int max) {    
    // Return random int from the range.
    // Use to blink led with random values.
    return min + (esp_random() % (max - min + 1));
}

void led_control_pixel(int i, led_strip_handle_t led_strip) {
    // Simple colour
    int RED = random_int_range(1, 255);
    int GREEN = random_int_range(1, 255);
    int BLUE = random_int_range(1, 255);
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, RED, GREEN, BLUE));
}

void led_control_hsv(int i, led_strip_handle_t led_strip) {
    // Color with saturation and brightness
    int hue = random_int_range(1, 360);  // MAX 360
    int saturation = random_int_range(1, 255); // MAX 255
    int brightLvl = random_int_range(1, 2);  // MAX 255
    ESP_ERROR_CHECK(led_strip_set_pixel_hsv(led_strip, i, hue, saturation, brightLvl));
}

void led_blink(bool led_on_off, led_strip_handle_t led_strip) {
    // Use to show board alive
    if (led_on_off) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        for (int i = 0; i < LED_STRIP_LED_COUNT; i++) {
            led_control_hsv(i, led_strip);
        }
        /* Refresh the strip to send data */
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        // ESP_LOGI(TAG, "LED ON!");
        // Beep pin
        pin_beep(led_on_off);
    } else {
        /* Set all LED off to clear all pixels */
        ESP_ERROR_CHECK(led_strip_clear(led_strip));
        // ESP_LOGI(TAG, "LED OFF!");
    }
}


// Led setup at APP_MAIN
// Before WHILE Loop
// led_strip_handle_t led_strip = configure_led();
// bool led_on_off = false;

// IN while lop
// while (1) {
//     // Led blink
//     led_blink(led_on_off, led_strip);
//     // Reset
//     led_on_off = !led_on_off;
//     // Sleep
//     vTaskDelay(pdMS_TO_TICKS(TIME_TICK_MS));
// }