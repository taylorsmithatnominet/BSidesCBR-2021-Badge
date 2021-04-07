if (NOT HAVE_AUDIO_CODEC)
    message(STATUS "Not building the audio driver")
    return()
endif()

add_library(dr_wav STATIC dr_wav.c)
target_include_directories(dr_wav PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dr_libs")
if (ESP32)
    # Some widechar stuff is in here somewhere so we need to depend on it :/
    target_link_libraries(dr_wav idf::esp_common)
endif()
