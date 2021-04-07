add_library(serialbridge STATIC
    serialbridge.cpp
    post.cpp
)
target_include_directories(serialbridge
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(serialbridge
    arduinocore
    keyboard
    BadgeOs
    SerialBridge
    diagnostics
)
