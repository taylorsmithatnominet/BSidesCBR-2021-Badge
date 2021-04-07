if (NOT HAVE_AUDIO_CODEC)
    message(STATUS "Not building the audio app")
    return()
endif()

add_library(audio STATIC
    audio.cpp
)
target_include_directories(audio
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(audio
    arduinocore
    idf::audiobit
    dr_wav
    BadgeOs
    menu
    display
)
list(APPEND BADGE_APP_LIBS audio)
set(BADGE_APP_LIBS "${BADGE_APP_LIBS}" PARENT_SCOPE)
