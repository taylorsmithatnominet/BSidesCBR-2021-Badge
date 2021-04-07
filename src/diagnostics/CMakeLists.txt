add_library(diagnostics STATIC
    diagnostics.cpp
    DiagApp.cpp
    ExampleDiagApp.cpp
)
target_include_directories(diagnostics
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(diagnostics
    arduinocore
    BSidesBadge2020
    BadgeOs
    display
    serialbridge
    rgbled
    menu
    irblaster_post
    ${BADGE_APP_LIBS}
)
