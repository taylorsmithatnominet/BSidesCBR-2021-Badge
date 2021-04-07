add_library(tetris STATIC
    tetris.cpp
)
target_include_directories(tetris
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(tetris
    arduinocore
    BadgeOs
    menu
    tetrisc
)
