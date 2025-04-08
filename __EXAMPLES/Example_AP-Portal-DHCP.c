#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_http_server.h"
#include "mdns.h"               //Required for captive portal
#include "dns_server.h"
#include "esp_netif.h"
#include "esp_netif_types.h"    //Required to set DHCP captive portal option
#include "globals.h"
#include "lwip/ip4_addr.h"      //Rrequired to change IP address to 10.10.10.1

static const char *TAG = "captive_portal";

// DNS server configuration
dns_server_config_t dns_config = DNS_SERVER_CONFIG_SINGLE("*", "WIFI_AP_DEF");

// HTTP handler for connectivity check (e.g., Android)
esp_err_t connectivity_check_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, "204 No Content");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// HTTP handler for iOS/macOS
esp_err_t apple_connectivity_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, "204 No Content");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// HTTP handler for Windows
esp_err_t windows_connectivity_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, "204 No Content");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// HTTP handler for Linux (NetworkManager)
esp_err_t linux_connectivity_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, "204 No Content");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}



// HTTP handler for the landing page
esp_err_t landing_page_handler(httpd_req_t *req) {
    const char *response = "<html><body><h1>Configure Your IoT Device</h1></body></html>";
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, response, strlen(response));
    ESP_LOGI(TAG,"Landing Page");
    return ESP_OK;
}

// Function to start the web server
void start_webserver() {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t connectivity_check_uri = {
        .uri        = "/generate_204",
        .method     = HTTP_GET,
        .handler    = connectivity_check_handler,
        .user_ctx   = NULL
    };

    httpd_uri_t connectivity_check_uri1 = {
        .uri        = "/generate204",
        .method     = HTTP_GET,
        .handler    = connectivity_check_handler,
        .user_ctx   = NULL
    };

    httpd_uri_t apple_connectivity_uri = {
        .uri         = "/hotspot-detect.html",
        .method     = HTTP_GET,
        .handler    = apple_connectivity_handler,
        .user_ctx   = NULL
    };

    httpd_uri_t windows_connectivity_uri = {
        .uri        = "/connecttest.txt",
        .method     = HTTP_GET,
        .handler    = windows_connectivity_handler,
        .user_ctx   = NULL
    };

    httpd_uri_t linux_connectivity_uri = {
        .uri        = "/check_network_status.txt",
        .method     = HTTP_GET,
        .handler    = linux_connectivity_handler,
        .user_ctx   = NULL
    };

    httpd_uri_t landing_page_uri = {
        .uri        = "/",
        .method     = HTTP_GET,
        .handler    = landing_page_handler,
        .user_ctx   = NULL
    };

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &connectivity_check_uri);
        httpd_register_uri_handler(server, &connectivity_check_uri1);
        httpd_register_uri_handler(server, &apple_connectivity_uri);
        httpd_register_uri_handler(server, &windows_connectivity_uri);
        httpd_register_uri_handler(server, &linux_connectivity_uri);
        httpd_register_uri_handler(server, &landing_page_uri);
    }
}

// Function to set DHCP option 114
void set_dhcp_option_114(esp_netif_t *netif) {
    esp_netif_dhcps_stop(netif);
    // Set DHCP option 114 
    char *venue_info_url = "http://10.10.10.1"; 
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI, venue_info_url, strlen(venue_info_url)));
    esp_netif_dhcps_start(netif);
}


void app_main() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *netif = esp_netif_create_default_wifi_ap();
    assert(netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Set the IP address for the AP
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, 10, 10, 10, 1);
    IP4_ADDR(&ip_info.gw, 10, 10, 10, 1);
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(netif));

    // Initialize mDNS 
    ESP_ERROR_CHECK(mdns_init()); 
    ESP_ERROR_CHECK(mdns_hostname_set("Example")); 
    ESP_ERROR_CHECK(mdns_instance_name_set("Example Captive Portal"));

    set_dhcp_option_114(netif);

    start_dns_server(&dns_config);
    start_webserver();
}

