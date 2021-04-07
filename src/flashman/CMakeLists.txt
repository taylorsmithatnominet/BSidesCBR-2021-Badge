add_library(flashman STATIC
    flashman.cpp
)
target_include_directories(flashman
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(flashman
    arduinocore
    BadgeLog
    SPIFlash
)
