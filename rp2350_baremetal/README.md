# RP2350 bare-metal LED project

This folder builds `C:/Users/twedgemo/Desktop/baremetal.cpp` for a Raspberry Pi
Pico 2 / RP2350 and produces a `.uf2` file.

The Pico SDK supplies the RP2350 startup code, linker script, boot metadata, and
UF2 conversion. Your C++ file still performs GPIO through direct register
access.

## One-time setup

Install:

- CMake
- Ninja or Make
- Arm GNU toolchain with `arm-none-eabi-g++`
- Raspberry Pi Pico SDK

Set `PICO_SDK_PATH` to your Pico SDK checkout.

PowerShell example:

```powershell
$env:PICO_SDK_PATH = "C:\pico\pico-sdk"
```

## Build

From this folder:

```powershell
cmake -S . -B build -G Ninja -DPICO_BOARD=pico2
cmake --build build
```

If you do not install Ninja, use a generator available on your machine instead.

The flashable file will be:

```text
build\rp2350_baremetal_leds.uf2
```

## Flash

1. Hold the Pico 2 `BOOTSEL` button.
2. Plug the board into USB.
3. Copy `build\rp2350_baremetal_leds.uf2` to the mounted `RPI-RP2` drive.

## Wiring assumed by the code

The program expects five active-high LEDs:

```text
LED 0 -> GPIO2
LED 1 -> GPIO3
LED 2 -> GPIO4
LED 3 -> GPIO5
LED 4 -> GPIO6
```

Use a current-limiting resistor for each LED.
