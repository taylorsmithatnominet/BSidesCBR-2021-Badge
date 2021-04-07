cmake_minimum_required(VERSION 3.12)

# Some features work on both the ESP32 and host build
if (ESP32 OR UNIX)
    list(APPEND FEATURES
        HAVE_SERIAL
        HAVE_DISPLAY
        HAVE_KEYBOARD
        # Stubbed for the host build
        HAVE_BLUETOOTH
        HAVE_WIFI
    )
endif()
# Some features are ESP32 only - `AND NOT` not strictly necessary here but
# useful to be explicit about the support
if (ESP32 AND NOT UNIX)
    # This ensures ESP32 functionality is not stubbed out
    add_compile_definitions(ESP32)
    list(APPEND FEATURES
        HAVE_SUSPEND
        HAVE_SD
        HAVE_FLASH
        HAVE_IMU
        HAVE_AUDIO_CODEC
    )
endif()
# Some hardware is attached to the SAMD
if (SAMD21 AND NOT UNIX)
    # This ensures SAMD functionality is not stubbed out
    add_compile_definitions(SAMD21)
    list(APPEND FEATURES
        HAVE_CAPSENSE
        HAVE_RGD_LEDC
        HAVE_IR
        HAVE_SAO
    )
endif()

# Add all of the features and also define CMake variables
add_compile_definitions(${FEATURES})
foreach(feature IN LISTS FEATURES)
    set("${feature}" TRUE)
endforeach()
