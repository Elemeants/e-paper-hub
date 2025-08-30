# E-Paper Portrait Hub

## Overview

E-Paper Portrait Hub is an ESP-IDF project that drives a Waveshare 4.2" e‑paper display (400×300, B/W/Red), reads raw images from a microSD card, and renders them using a lightweight 2‑plane graphics buffer with simple drawing/text APIs. It also reads battery percentage from a MAX17048 fuel gauge.

## What’s Included

- Waveshare 4.2" SPI display driver with busy/refresh handling.
- Graphics frame buffer and renderer (black and red planes), with primitives:
  - pixels, lines, rectangles (stroke/fill), bitmap blitting, and text (fixed 16px font).
- microSD (SPI) driver with VFS FAT mount at `/sdcard`.
- MAX17048 I2C battery driver returning SoC percentage.
- Hub orchestration that initializes peripherals, discovers images on the SD card, and cycles them on screen.

## How It Works

- On boot, the hub initializes battery, display, and SD drivers, attaches a frame buffer to the renderer, and scans the SD root for images.
- Any file on `/sdcard` with extension `.bin` or `.BIN` is appended to a linked list and shown in sequence.
- Each loop iteration loads the next image into the frame buffer and refreshes the display. Default delay is 10 seconds between images.
- Battery percentage is printed over serial on startup.

## Image Format

- Expected size: 400×300 pixels.
- Layout: raw 1‑bit per pixel bitmap, MSB‑first per byte, row‑major.
- File size: `(400 / 8) * 300 = 15,000` bytes.
- Placement: copy `.bin` files to the SD root (`/sdcard`).

Tip: If you are generating bitmaps yourself, ensure the 1bpp packing matches MSB‑first, 8 pixels per byte, rows contiguous.

## Hardware and Pins

Default pin configuration is set in `src/main.c` (edit to match your wiring):

- Display (SPI, Waveshare 4.2): `SPI2_HOST` with `CLK=GPIO19`, `MOSI=GPIO23`, `MISO=GPIO25`, `CS=GPIO22`, `BUSY=GPIO25`, `RST=GPIO26`, `DC=GPIO27`.
- SD card (SPI): `SPI3_HOST` with `CLK=GPIO5`, `MOSI=GPIO17`, `MISO=GPIO16`, `CS=GPIO18`.
- Battery (MAX17048 I2C): `SDA=GPIO14`, `SCL=GPIO13`.

Note: Adjust pins in `src/main.c` to your board; the display driver also requires a valid BUSY pin and proper reset/CS/DC wiring.

## Build and Flash

This project uses [PlatformIO](https://platformio.org/) with the ESP‑IDF framework.

- Open the folder in VS Code with the PlatformIO extension, select the `esp32doit-devkit-v1` environment, and build/flash.
- Build flags are set in `platformio.ini` and include:
  - Automatic `FIRMWARE_VERSION` from git via `utils/git_rev.py`.
  - `BUILD_TIME_UNIX` from the environment variable `UNIX_TIME`.

Requirements:

- Git available in your PATH for the version script. If unavailable, `FIRMWARE_VERSION` falls back to `"UNKNOWN"`.
- An SD card formatted as FAT, inserted and wired per the pins above.

## Running

1. Prepare SD card with one or more 400×300 1bpp `.bin` images in the root directory.
2. Flash the firmware and open the serial monitor.
3. On boot, firmware prints version and battery %, then cycles images every ~10 seconds.

## Code Structure

- `src/hub.c` and `src/hub.h`: Peripheral init, SD scan, image loop.
- `src/screen/frame.{h,c}`: Frame buffer and drawing primitives (pixels, lines, rects, text, bitmap).
- `src/screen/renderer.{h,c}`: Converts frame buffer to device planes and pushes to display.
- `src/drivers/display/waveshare_42in_spi_driver.{h,c}`: Display SPI driver and command set.
- `src/drivers/sdcard/sd_spi_driver.{h,c}`: SPI + VFS FAT mount at `/sdcard`.
- `src/drivers/battery/max17048_i2c_driver.{h,c}`: MAX17048 I2C driver and SoC read.
- `src/fonts/` and `src/test_image.{h,c}`: Built‑in font and sample image.

## Roadmap

- [x] Separate frame/display buffer from the Waveshare driver
- [x] microSD support to load images
- [x] Frame buffer primitives and text rendering
- [ ] Wi‑Fi connectivity to receive images
- [ ] Basic dithering for improved B/W output

## Author

[Daniel Polanco](https://github.com/Elemeants)
