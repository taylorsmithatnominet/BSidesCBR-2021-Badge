add_library(helloworld STATIC
    helloworld.cpp
)
target_include_directories(helloworld
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(helloworld
    arduinocore
    BSidesBadge2020
    BadgeOs
    display
    menu
)
