if (NOT HAVE_SUSPEND)
    message(STATUS "Not building the suspend service")
    return()
endif()

add_library(suspend STATIC
    suspend.cpp
)
target_include_directories(suspend
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(suspend
    arduinocore
    BadgeLog
    BadgeOs
    BSidesBadge2020
    BSidesMessaging
    serialbridge
    display
    keyboard
    audio
)
list(APPEND BADGE_APP_LIBS suspend)
set(BADGE_APP_LIBS "${BADGE_APP_LIBS}" PARENT_SCOPE)
