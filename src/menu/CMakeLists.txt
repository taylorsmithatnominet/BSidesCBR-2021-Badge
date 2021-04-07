add_library(menu STATIC
    menu.cpp
)
target_include_directories(menu
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(menu
    arduinocore
    BadgeOs
    display
    BadgeOs
    BadgeAssets
    schedule
    BadgeSchedule
)
if (HAVE_SUSPEND)
target_link_libraries(menu
    suspend
)
endif()
