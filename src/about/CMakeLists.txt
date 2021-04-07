add_library(about STATIC
    about.cpp
)
target_include_directories(about
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(about
    arduinocore
    BSidesBadge2020
    BadgeOs
    display
    menu
    device_info
)
