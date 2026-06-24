# This is a compact Pico SDK import helper. Set PICO_SDK_PATH to the local
# pico-sdk checkout before configuring this project.

if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

if (NOT PICO_SDK_PATH)
    message(FATAL_ERROR
        "PICO_SDK_PATH is not set. Set it to your pico-sdk checkout, for example: "
        "cmake -S . -B build -DPICO_SDK_PATH=C:/pico/pico-sdk")
endif()

get_filename_component(PICO_SDK_PATH "${PICO_SDK_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

set(PICO_SDK_INIT_CMAKE_FILE "${PICO_SDK_PATH}/pico_sdk_init.cmake")

if (NOT EXISTS "${PICO_SDK_INIT_CMAKE_FILE}")
    message(FATAL_ERROR
        "Could not find pico_sdk_init.cmake at ${PICO_SDK_INIT_CMAKE_FILE}. "
        "Check PICO_SDK_PATH.")
endif()

include("${PICO_SDK_INIT_CMAKE_FILE}")
