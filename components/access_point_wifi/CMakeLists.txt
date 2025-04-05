# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#minimal-component-cmakelists
# Edit following two lines to set component requirements (see docs)
idf_component_register(SRCS "access_point_wifi.c"
                  INCLUDE_DIRS "include"
                  REQUIRES esp_wifi nvs_flash)