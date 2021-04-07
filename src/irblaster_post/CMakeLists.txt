add_library(irblaster_post STATIC
    irblaster_post.h
    irblaster_post.cpp
)

target_include_directories(irblaster_post
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(irblaster_post
    keyboard
    BadgeOs
    SerialBridge
    diagnostics
)
