add_library(sketch STATIC
    sketch.cpp
)
target_include_directories(sketch
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
if(HAVE_IMU)
    set(ADDITIONAL_ITEMS SixAxisMotionDriver)
endif()
target_link_libraries(sketch
    arduinocore
    BSidesBadge2020
    BadgeOs
    display
    menu
    ${ADDITIONAL_ITEMS}
)
