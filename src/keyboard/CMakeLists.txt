add_library(keyboard STATIC
    keyboard.cpp
)
target_include_directories(keyboard
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(keyboard
    arduinocore
    BadgeOs
    BSidesBadge2020
    BSidesMessaging
)
