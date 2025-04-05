# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#minimal-component-cmakelists
# Edit following two lines to set component requirements (see docs)
idf_component_register(SRCS "littlefs_init.c"
                  INCLUDE_DIRS "include"
                  REQUIRES esp_system 
                #   PRIV_REQUIRES esp_littlefs
                  )