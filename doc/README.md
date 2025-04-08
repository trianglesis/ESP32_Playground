# Notes

Board used ESP32 C6 Devkit C-1
- https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html#hardware-reference

Try to use only IDF, no arduino.

## Web Server

### Wifi AP

Works in general

Soft AP
https://github.com/espressif/esp-idf/blob/v5.4.1/examples/wifi/getting_started/softAP/main/softap_example_main.c

No password: 
https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-reference/network/esp_wifi.html#_CPPv416wifi_auth_mode_t
https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/api-guides/wifi-security.html

### Serve files

Works in general

1. Create simple partition table first: `idf.py menuconfig` and select 

- `Single factory APP no OTA` OR `Single factory app (large) no OTA`

1. Generate CSV refference next: 

- Gen cmd: `idf.py partition-table` copy generated below
- - Doc: https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-guides/partition-tables.html#generating-binary-partition-table


```text
#### Usual

Partition table binary generated. Contents:
*******************************************************************************
# ESP-IDF Partition Table
# Name, Type, SubType, Offset, Size, Flags
nvs,data,nvs,0x9000,24K,
phy_init,data,phy,0xf000,4K,
factory,app,factory,0x10000,1M,
*******************************************************************************

#### LARGE
*******************************************************************************
# ESP-IDF Partition Table
# Name, Type, SubType, Offset, Size, Flags
nvs,data,nvs,0x9000,24K,
phy_init,data,phy,0xf000,4K,
factory,app,factory,0x10000,1500K,
*******************************************************************************
```

3. New partition for Little FS:

- Copy content into new file at root dir `partitions.csv`
- Add new partition for `littlefs`:
- Optionally change size to simple `1M`, `2M` up to `4M` (I have 8M ESP32 C6)
- Additionally check: **KCONFIG Name:** `ESPTOOLPY_FLASHSIZE` to show youe board actual flash

```csv
# Common partitions above.
# This string was copied from littleFS example: partitions_demo_esp_littlefs.csv
littlefs,  data, littlefs,      ,  0xF0000, 
```

Optional:
- I tried to increase the fs size, since I have 8Mb to spare

```csv
littlefs,data,littlefs,,4M,
```
Compiling...
Working fine:

[Log](logs/partition_and_wifi.log)


4. Try to build with new partition tables.

- Use `idf.py menuconfig` and set `Partition Table` use `Custom partition table CSV` to `partitions.csv`

##### Doc

Partitions and sizes: 
- https://gitdemo.readthedocs.io/en/latest/partition-tables.html#offset-size
- https://docs.espressif.com/projects/esp-at/en/latest/esp32c6/Compile_and_Develop/How_to_customize_partitions.html

FS:
- https://components.espressif.com/components/joltwallet/littlefs/versions/1.19.1
- https://github.com/joltwallet/esp_littlefs
- https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html


### Create index.html

Use just simple web page.

Now following: https://randomnerdtutorials.com/esp32-esp8266-plot-chart-web-server/


# Web server

### All on One WIfiAP, Wifi fallback, HTTP WEB Server

- https://github.com/tonyp7/esp32-wifi-manager

Can override a built-in webserver with personal pages
- https://github.com/tonyp7/esp32-wifi-manager?tab=readme-ov-file#interacting-with-the-http-server

### All in one:

- https://github.com/elliotmade/ESP32-Captive-Portal-Example


### Async WEBServer

**NOTE** Cannot use as clean IDF setup, only for arduino?

- https://stackoverflow.com/questions/69266035/how-to-set-up-an-async-web-server-with-esp-idf

Must install both:
- AsyncTCP
- ESPAsyncWebServer

Try to set up and understand why I may need this:
- https://github.com/ESP32Async/ESPAsyncWebServer/wiki#how-to-install

Use other examples for faster dive in:
- https://randomnerdtutorials.com/esp32-esp8266-plot-chart-web-server/

Install dep and solve issues:

```test
# root dir file: sdkconfig
CONFIG_FREERTOS_HZ=1000
```

### IDF Standart httpd

- https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
- https://github.com/espressif/esp-idf/tree/5c51472e/examples/protocols/http_server/simple
- https://esp32tutorials.com/esp32-esp-idf-websocket-web-server/

REST seems suitable (or not):
- https://github.com/espressif/esp-idf/tree/5c51472e/examples/protocols/http_server/restful_server

Reuse captive portal to better show WEB page as soon as phone connects to WiFi AP
- https://github.com/espressif/esp-idf/blob/5c51472e82a58098dda8d40a1c4f250c374fc900/examples/protocols/http_server/captive_portal/main/main.c

#### Ideas

Copy:
Example DNS server from `PROJECT_ROOT\v5.4.1\esp-idf\examples\protocols\http_server\captive_portal\components`
to `PROJECT_ROOT\v5.4.1\esp-idf\components\dns_server`


Problems:
- https://esp32.com/viewtopic.php?t=44005
  
#### Working example

- `\v5.4.1\esp-idf\examples\protocols\http_server\captive_portal\main\main.c`

Reuse `root_get_handler` with `load_index_file_buffer`

Captive redirection does not work yet

"Header Fields are too long for the server to interpret"
- https://stackoverflow.com/a/67849714
- https://github.com/tonyp7/esp32-wifi-manager/issues/147

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



### Other examples
- https://github.com/ESP32Tutorials/ESP32-ESP-IDF-SPIFFS-Web-Server

# SQL !

Nice to have it on SD card to store data for a few days, weeks and show at LVGL display: pages
Not tested it yet, will try after LVGL

- https://github.com/nopnop2002/esp32-idf-sqlite3
