add_library(schedule STATIC
    schedule.cpp
)
target_include_directories(schedule
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(schedule
    arduinocore
    BSidesBadge2020
    BadgeAssets
    BadgeOs
    display
    menu
)
