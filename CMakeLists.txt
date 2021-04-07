cmake_minimum_required(VERSION 3.5)
project(bsidesBadge2020)

set(ESP_IDF_SDKCONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/badge/sdkconfig" CACHE INTERNAL "esp-idf sdkconfig file")
# We need to define this early before the IDF and our ESP32 cmake stuff runs
list(APPEND EXTRA_COMPONENT_DIRS
    "${CMAKE_SOURCE_DIR}/arduino-libs/AudioDriver/ESP32_SGTL5000_driver/components"
)

add_subdirectory(external)

# Set up platform specific features and compile flags
include("${CMAKE_SOURCE_DIR}/tools/platform.cmake")

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -DCONFIG_BADGEOS_LOG_LEVEL=LOG_DEBUG"
)
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -DCONFIG_BADGEOS_LOG_LEVEL=LOG_DEBUG"
)

add_subdirectory(arduino-libs)
add_subdirectory(src)
