function(add_arduino_library_glob_prefix TARGET_NAME TARGET_PATH GLOB_PREFIX EXTRA_PREPROCESSOR_FLAGS)
    message(STATUS "Adding Arduino library ${TARGET_NAME}")
    set(src "${TARGET_PATH}")
    if(IS_DIRECTORY "${src}/src")
        set(src "${src}/src")
    endif()
    file(GLOB src_files "${src}/${GLOB_PREFIX}.cpp" "${src}/${GLOB_PREFIX}.c" )
    set(blank "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/blank.cpp")
    if(NOT EXISTS "${blank}")
        file(WRITE ${blank} "")
    endif()
    add_definitions(${EXTRA_PREPROCESSOR_FLAGS})
    add_library(${TARGET_NAME} STATIC
        ${src_files}
        ${blank}
    )
    target_link_libraries(${TARGET_NAME}
        arduinocore
    )
    target_include_directories(${TARGET_NAME}
        PUBLIC ${src}
    )
endfunction()

# Libraries required by all components
add_arduino_library_glob_prefix(crc32
    ${CMAKE_CURRENT_SOURCE_DIR}/crc32
    Crc32
    "-D__BYTE_ORDER=1234"
)
add_arduino_library(BadgeLog
    ${CMAKE_CURRENT_SOURCE_DIR}/BadgeLog
)
add_arduino_library(BSidesBadge2020
    ${CMAKE_CURRENT_SOURCE_DIR}/BSidesBadge2020
)
add_arduino_library(CircularBuffer
    ${CMAKE_CURRENT_SOURCE_DIR}/CircularBuffer
)
add_arduino_library(BSidesMessaging
    ${CMAKE_CURRENT_SOURCE_DIR}/BSidesMessaging
)
target_link_libraries(BSidesMessaging
    CircularBuffer
    crc32
)
add_arduino_library(SerialBridge
    ${CMAKE_CURRENT_SOURCE_DIR}/SerialBridge
)
target_link_libraries(SerialBridge
    BSidesBadge2020
    BSidesMessaging
    BadgeLog
)

# ESP / host only
if (ESP32 OR UNIX)
add_arduino_library(Adafruit-GFX-Library
    ${CMAKE_CURRENT_SOURCE_DIR}/Adafruit-GFX-Library
)
target_link_libraries(Adafruit-GFX-Library
    SPI
)
add_arduino_library(Adafruit_EPD
    ${CMAKE_CURRENT_SOURCE_DIR}/Adafruit_EPD
)
target_link_libraries(Adafruit_EPD
    Adafruit-GFX-Library
)
add_arduino_library(Adafruit-X11
    ${CMAKE_CURRENT_SOURCE_DIR}/Adafruit-X11
)
target_link_libraries(Adafruit-X11
    Adafruit-GFX-Library
)
add_arduino_library(Adafruit-Cache
    ${CMAKE_CURRENT_SOURCE_DIR}/Adafruit-Cache
)
target_link_libraries(Adafruit-Cache
    Adafruit-GFX-Library
)
add_arduino_library(Adafruit-SSD1675BX
    ${CMAKE_CURRENT_SOURCE_DIR}/Adafruit-SSD1675BX
)
target_link_libraries(Adafruit-SSD1675BX
    Adafruit-GFX-Library
    Adafruit_EPD
)
add_arduino_library(SPIFlash
    ${CMAKE_CURRENT_SOURCE_DIR}/SPIFlash
)
target_link_libraries(SPIFlash
    SPI
)

add_arduino_library(BadgeOs
    ${CMAKE_CURRENT_SOURCE_DIR}/BadgeOs
)
target_link_libraries(BadgeOs
    viewerc
    BadgeLog
)
add_arduino_library(Pacman
    ${CMAKE_CURRENT_SOURCE_DIR}/Pacman
)
add_arduino_library(viewerc
    ${CMAKE_CURRENT_SOURCE_DIR}/viewerc
)
add_arduino_library(BadgeAssets
    ${CMAKE_CURRENT_SOURCE_DIR}/BadgeAssets
)
target_link_libraries(BadgeAssets
    Adafruit-GFX-Library
)
add_arduino_library(HackerNames
    ${CMAKE_CURRENT_SOURCE_DIR}/HackerNames
)
add_arduino_library(BadgeInfo
    ${CMAKE_CURRENT_SOURCE_DIR}/BadgeInfo
)
target_link_libraries(BadgeInfo
    HackerNames
    SerialBridge
    BSidesMessaging
    BadgeOs
)
add_arduino_library(BadgeSchedule
    ${CMAKE_CURRENT_SOURCE_DIR}/BadgeSchedule
)
add_arduino_library(snakec
    ${CMAKE_CURRENT_SOURCE_DIR}/snakec
)
add_arduino_library(tetrisc
    ${CMAKE_CURRENT_SOURCE_DIR}/tetrisc
)
endif()

# ESP-specific
if(ESP32)
    target_link_libraries(SPIFlash
        idf::spi_flash
    )
endif()

add_subdirectory(AudioDriver)

if (HAVE_IMU)
    add_arduino_library(SixAxisMotionDriver
        ${CMAKE_CURRENT_SOURCE_DIR}/SixAxisMotionDriver
    )
    target_link_libraries(SixAxisMotionDriver
        Wire
    )
else()
    message(STATUS "Not building the IMU driver")
endif(HAVE_IMU)

# host-specific
if (UNIX)
    # hack to make it compile
    target_compile_definitions(Adafruit_EPD
        PRIVATE ESP32
    )
    target_link_libraries(Adafruit-X11
        X11
    )
endif()

# SAMD only
if (SAMD21)
add_arduino_library(AceRoutine
    ${CMAKE_CURRENT_SOURCE_DIR}/AceRoutine/src
)
set(src "${CMAKE_CURRENT_SOURCE_DIR}/AceRoutine/src/ace_routine")
file(GLOB srcs "${src}/*.cpp" "${src}/*.c")
target_sources(AceRoutine
    PRIVATE ${srcs}
)
add_arduino_library(ArduinoLowPower
    ${CMAKE_CURRENT_SOURCE_DIR}/ArduinoLowPower/src/
)
set (src "${CMAKE_CURRENT_SOURCE_DIR}/ArduinoLowPower/src/samd")
file (GLOB srcs "${src}/*.cpp" "${src}/*.c")
target_sources(ArduinoLowPower
    PRIVATE ${srcs}
)
target_link_libraries(ArduinoLowPower
    RTCZero
)
add_arduino_library(Adafruit_FreeTouch
    ${CMAKE_CURRENT_SOURCE_DIR}/Adafruit_FreeTouch/
)
add_arduino_library(RTCZero
    ${CMAKE_CURRENT_SOURCE_DIR}/RTCZero/src
)
add_arduino_library(RgbDriver
    ${CMAKE_CURRENT_SOURCE_DIR}/RgbDriver
)
target_link_libraries(RgbDriver
    Wire
)
target_link_libraries(BadgeLog
    SerialBridge
)
add_arduino_library(Arduino-IRremote
    ${CMAKE_CURRENT_SOURCE_DIR}/Arduino-IRremote
)
add_arduino_library(IRBlaster
    ${CMAKE_CURRENT_SOURCE_DIR}/IRBlaster
)
target_link_libraries(IRBlaster
    Arduino-IRremote
)
endif()
