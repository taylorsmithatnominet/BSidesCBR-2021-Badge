add_library(snake STATIC
    snake.cpp
)
target_include_directories(snake
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(snake
    arduinocore
    BadgeOs
    menu
    snakec
)
