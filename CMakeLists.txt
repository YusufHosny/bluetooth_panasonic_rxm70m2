cmake_minimum_required(VERSION 3.16.0)

# PlatformIO installs arduino-audio-tools as 'audio-tools', but ESP32-A2DP's
# CMakeLists.txt requires the component by its original name. Symlink at
# configure time so IDF sees both names resolved to the same directory.
set(LIBDEPS "${CMAKE_SOURCE_DIR}/.pio/libdeps/esp32dev")
if(EXISTS "${LIBDEPS}/audio-tools" AND NOT EXISTS "${LIBDEPS}/arduino-audio-tools")
    file(CREATE_LINK "${LIBDEPS}/audio-tools" "${LIBDEPS}/arduino-audio-tools" SYMBOLIC)
endif()

list(APPEND EXTRA_COMPONENT_DIRS
    "${LIBDEPS}/ESP32-A2DP"
    "${LIBDEPS}/arduino-audio-tools"
    "${LIBDEPS}/libhelix"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(bluetooth-panasonic-rxm70m2)

# Propagate platform defines to all IDF components.
# ESP32-A2DP checks for 'ESP32' and won't compile without it.
# audio-tools exposes ESP32_CMAKE as INTERFACE but IDF doesn't always propagate
# INTERFACE flags across component boundaries, so we set them globally.
idf_build_set_property(COMPILE_DEFINITIONS "ESP32=1"                  APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "ESP32_CMAKE=1"            APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "CONFIG_IDF_TARGET_ESP32=1" APPEND)
