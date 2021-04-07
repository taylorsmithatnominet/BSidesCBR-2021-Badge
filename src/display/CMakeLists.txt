add_library(display STATIC
    display.cpp
    post.cpp
)
target_include_directories(display
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(display
    arduinocore
    BSidesBadge2020
    BadgeOs
    BadgeAssets
    diagnostics
)
if(ESP32)
    target_link_libraries(display
        Adafruit-SSD1675BX
    )
endif()
if(UNIX)
    target_link_libraries(display
        Adafruit-X11
        Adafruit-Cache
    )
endif()
