FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS         ${app_sources}
    INCLUDE_DIRS "../include" "../assets"
    REQUIRES     ESP32-A2DP arduino-audio-tools libhelix nvs_flash bt freertos driver
)
