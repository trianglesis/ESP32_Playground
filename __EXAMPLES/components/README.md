# Submodules

Some extra imports does not work here.
Leaving this multimodular idea for later, untill I get to understand it better.

```log
D:/Projects/ESP/projects/ESP32-C6/playground/project-name/components/littlefs_init/littlefs_init.c:8:10: fatal error: esp_littlefs.h: No such file or directory
    8 | #include "esp_littlefs.h"
      |          ^~~~~~~~~~~~~~~~
compilation terminated.
```

OR

```log
CMake Error at D:/Projects/ESP/Espressif/v5.4.1/esp-idf/tools/cmake/build.cmake:318 (message):
  Failed to resolve component 'esp_littlefs' required by component
  'littlefs_init': unknown name.
Call Stack (most recent call first):
  D:/Projects/ESP/Espressif/v5.4.1/esp-idf/tools/cmake/build.cmake:367 (__build_resolve_and_add_req)
  D:/Projects/ESP/Espressif/v5.4.1/esp-idf/tools/cmake/build.cmake:665 (__build_expand_requirements)
  D:/Projects/ESP/Espressif/v5.4.1/esp-idf/tools/cmake/project.cmake:717 (idf_build_process)
  CMakeLists.txt:8 (project)


-- Configuring incomplete, errors occurred!
ninja: error: rebuilding 'build.ninja': subcommand failed

FAILED: build.ninja
```