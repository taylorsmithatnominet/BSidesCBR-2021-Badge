add_library(pacman STATIC
    pacman.cpp
)
target_include_directories(pacman
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(pacman
    arduinocore
    BSidesBadge2020
    BadgeOs
    menu
    Pacman
)
