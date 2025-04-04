# Notes

## Web Server

### Async WEBServer

Try to set up and understand why I may need this:
- https://github.com/ESP32Async/ESPAsyncWebServer/wiki#how-to-install

Install dep and solve issues:

```test
# root dir file: sdkconfig
CONFIG_FREERTOS_HZ=1000
```

### IDF

Official IDF doc:
- https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/esp_http_server.html