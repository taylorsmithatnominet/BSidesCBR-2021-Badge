add_library(rgbled STATIC
    rgbled.cpp
)
target_include_directories(rgbled
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(rgbled
    arduinocore
    BSidesBadge2020
    BadgeOs
    diagnostics
    display
    menu
)
