cmake_minimum_required(VERSION 3.13)

# Build this for Raspberry Pi Pico 2 / RP2350 unless the user overrides it.
set(PICO_BOARD pico2 CACHE STRING "Target board")

include(pico_sdk_import.cmake)

project(rp2350_baremetal_leds C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()

set(BAREMETAL_SOURCE
    "C:/Users/twedgemo/Desktop/baremetal.cpp"
    CACHE FILEPATH "Path to the direct-register LED source file")

add_executable(rp2350_baremetal_leds
    ${BAREMETAL_SOURCE}
)

# Your program uses only direct MMIO and does not need stdio transports.
pico_enable_stdio_usb(rp2350_baremetal_leds 0)
pico_enable_stdio_uart(rp2350_baremetal_leds 0)

# Keep the SDK startup/linker/UF2 support. stdio is disabled above, so this is
# mostly the board/runtime setup needed to enter main() cleanly.
target_link_libraries(rp2350_baremetal_leds
    pico_stdlib
)

pico_add_extra_outputs(rp2350_baremetal_leds)
