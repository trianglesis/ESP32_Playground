# Notes

## Web Server

### Async WEBServer

Try to set up and understand why I may need this:
- https://github.com/ESP32Async/ESPAsyncWebServer/wiki#how-to-install

Use other examples for faster dive in:
- https://randomnerdtutorials.com/esp32-esp8266-plot-chart-web-server/

Install dep and solve issues:

```test
# root dir file: sdkconfig
CONFIG_FREERTOS_HZ=1000
```

### IDF

Official IDF doc:
- https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/esp_http_server.html

Mesh: 
- https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-guides/esp-wifi-mesh.html

Networking:
- https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/migration-guides/release-5.x/5.0/networking.html

#### Wifi:

Examples ready:
https://github.com/espressif/esp-idf/tree/v5.4.1/examples/wifi

Wifi Station:
- https://github.com/espressif/esp-idf/blob/v5.4.1/examples/wifi/getting_started/station/main/station_example_main.c

Doc

Driver:
- https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-guides/wifi.html
Scan and connect
- https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/migration-guides/release-5.x/5.4/wifi.html
AP
- https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-reference/network/esp_wifi.html

Captive portal:
- https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-reference/provisioning/wifi_provisioning.html