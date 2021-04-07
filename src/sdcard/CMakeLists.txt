add_library(sdcard STATIC
    sdcard.cpp
)
target_include_directories(sdcard
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(sdcard
    arduinocore
    BadgeOs
    BSidesBadge2020
)
if (ESP32)
    target_link_libraries(sdcard
        SD
    )
endif()
