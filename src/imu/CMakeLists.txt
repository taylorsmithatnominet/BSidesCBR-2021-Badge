if (NOT HAVE_IMU)
    message(STATUS "Not building the IMU app")
    return()
endif()

add_library(imu STATIC
    imu.cpp
    imu_post.cpp
)
target_include_directories(imu
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(imu
    arduinocore
    BSidesBadge2020
    BadgeOs
    diagnostics
    display
    menu
    SixAxisMotionDriver
)
list(APPEND BADGE_APP_LIBS imu)
set(BADGE_APP_LIBS "${BADGE_APP_LIBS}" PARENT_SCOPE)
