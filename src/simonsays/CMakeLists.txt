add_library(simonsays STATIC
    simonsays.cpp
)
target_include_directories(simonsays
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(simonsays
    arduinocore
    BSidesBadge2020
    BSidesMessaging
    serialbridge
    BadgeOs
    display
    menu
)
