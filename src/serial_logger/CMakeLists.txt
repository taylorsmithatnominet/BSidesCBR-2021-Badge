add_library(serial STATIC
    serial.cpp
)
target_include_directories(serial
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(serial
    arduinocore
    BadgeOs
    BadgeLog
)
