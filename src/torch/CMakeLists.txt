add_library(torch STATIC
    torch.cpp
)
target_include_directories(torch
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(torch
    arduinocore
    BSidesBadge2020
    BadgeOs
    display
    menu
)
