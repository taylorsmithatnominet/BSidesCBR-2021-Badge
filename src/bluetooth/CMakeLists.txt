add_library(bluetooth STATIC
    bluetooth.cpp
)
target_include_directories(bluetooth
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(bluetooth
    arduinocore
    BadgeOs
    display
    menu
    BadgeInfo
    BadgeAssets
)
if (ESP32)
    target_link_libraries(bluetooth
        BluetoothSerial
        BLE
    )
endif()
