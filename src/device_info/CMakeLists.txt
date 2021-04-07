add_library(device_info STATIC
    device_info.cpp
)
target_include_directories(device_info
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(device_info
    arduinocore
    BadgeOs
    BadgeAssets
    BadgeInfo
    display
)
