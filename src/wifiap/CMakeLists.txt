add_library(wifiap STATIC
    wifiap.cpp
)
target_include_directories(wifiap
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(wifiap
    arduinocore
    BadgeOs
    display
    menu
    BadgeInfo
    BadgeAssets
    BadgeLog
)
if (ESP32)
    target_link_libraries(wifiap
        WiFi
    )
endif()
