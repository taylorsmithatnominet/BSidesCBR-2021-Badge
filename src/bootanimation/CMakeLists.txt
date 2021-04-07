add_library(bootanimation STATIC
    bootanimation.cpp
)
target_include_directories(bootanimation
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(bootanimation
    arduinocore
    diagnostics
    display
    BadgeOs
    BadgeAssets
)
