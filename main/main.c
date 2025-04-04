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
// 
// WiFI AP
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"
// My custom
// LED
#include "led_strip.h"
// GPIO
#include "driver/gpio.h"
// Random colours gen
#include "esp_random.h"
// FS
#include "spi_flash_mmap.h"
#include "esp_littlefs.h"

// WEBSERVER
#include "esp_http_server.h"
#include "mdns.h"               //Required for captive portal
#include "dns_server.h"
#include "esp_netif_types.h"    //Required to set DHCP captive portal option
#include "globals.h"
#include "lwip/ip4_addr.h"      //Rrequired to change IP address to 10.10.10.1

// DNS server configuration
dns_server_config_t dns_config = DNS_SERVER_CONFIG_SINGLE("*", "WIFI_AP_DEF");

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

// For ESP32 C6 with integrated LED
// Init the LED as a usefull indicator of a working cycle.
// In addition use 3v Beeper and one GPIO as a lightweight tester for PIN IOs =)

#define EXAMPLE_ESP_WIFI_SSID      "ESP32_test_ap"
#define EXAMPLE_ESP_WIFI_PASS      "" // Empty str len 0 = No password, this AP is KIOSK-like
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       10


// Do not used
#define LED_STRIP_USE_DMA 0

// LED OPTS
#define TIME_TICK_MS 500

#define LED_STRIP_LED_COUNT 1
#define LED_STRIP_MEMORY_BLOCK_WORDS 0 // let the driver choose a proper memory block size automatically

#define LED_STRIP_GPIO_PIN 8
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)

static const char *TAG = "playground";

// WIfi AP
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data) {
        if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
        MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
        MAC2STR(event->mac), event->aid, event->reason);
    }
}

// On AP
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

void wifi_init_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_OPEN,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

// Android redirect?
static void dhcp_set_captiveportal_url(void) {
    // get the IP of the access point to redirect to
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);

    // turn the IP into a URI
    char* captiveportal_uri = (char*) malloc(32 * sizeof(char));
    assert(captiveportal_uri && "Failed to allocate captiveportal_uri");
    strcpy(captiveportal_uri, "http://");
    strcat(captiveportal_uri, ip_addr);

    // get a handle to configure DHCP with
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    // set the DHCP option 114
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_stop(netif));
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI, captiveportal_uri, strlen(captiveportal_uri)));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_start(netif));
}

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

// Webserver conf

// HTTP GET Handler
static esp_err_t root_get_handler(httpd_req_t *req)
{
    const uint32_t root_len = root_end - root_start;

    ESP_LOGI(TAG, "Serve root");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, root_start, root_len);

    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler
};

// HTTP Error (404) Handler - Redirects all requests to the root page
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGI(TAG, "Redirecting to root");
    return ESP_OK;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 13;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    return server;
}

//  Start BIP, start LED, cycle both
void app_main(void)
{
    // Setup BIP pin
    beep_setup();
    // Led setup
    led_strip_handle_t led_strip = configure_led();
    bool led_on_off = false;
    ESP_LOGI(TAG, "Start blinking LED and Beep");
 
    // Start Wifi AP
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();

    // FS Setup
    fs_setup();
    // Read file
    fs_read();

    // WebServer
    /*
        Turn of warnings from HTTP server as redirecting traffic will yield
        lots of invalid requests
    */
   esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
   esp_log_level_set("httpd_txrx", ESP_LOG_ERROR);
   esp_log_level_set("httpd_parse", ESP_LOG_ERROR);


   // Initialize networking stack
   ESP_ERROR_CHECK(esp_netif_init());

   // Create default event loop needed by the  main app
   ESP_ERROR_CHECK(esp_event_loop_create_default());

   // Initialize NVS needed by Wi-Fi
   ESP_ERROR_CHECK(nvs_flash_init());

   // Initialize Wi-Fi including netif with default config
   esp_netif_create_default_wifi_ap();

   // Initialise ESP32 in SoftAP mode
   wifi_init_softap();

   // Configure DNS-based captive portal, if configured
   #ifdef CONFIG_ESP_ENABLE_DHCP_CAPTIVEPORTAL
       dhcp_set_captiveportal_url();
   #endif

   // Start the server for the first time
   start_webserver();

   // Start the DNS server that will redirect all queries to the softAP IP
   dns_server_config_t config = DNS_SERVER_CONFIG_SINGLE("*" /* all A queries */, "WIFI_AP_DEF" /* softAP netif ID */);
   start_dns_server(&config);

    while (1) {
        // Led blink
        led_blink(led_on_off, led_strip);
        // Reset
        led_on_off = !led_on_off;
        // Sleep
        vTaskDelay(pdMS_TO_TICKS(TIME_TICK_MS));
    }
}