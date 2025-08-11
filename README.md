# E-Paper Portrait Hub

## Overview

The E-Paper Portrait Hub is a project aimed to provide an IoT device that can display your favorite pictures and information on its elegant, efficient, and low-power e-paper display.

## Features

- Support for 4.2inch 400x300 B/W/R E-Paper.
- Battery reading using MAX17048 IC.

## Build steps

This project is built using [PlatformIO](https://platformio.org/) and the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html), so you can simply clone the repository and open it in PlatformIO (ex: VSCode with PlatformIO extension), and build it from there.

## Author

[Daniel Polanco](https://github.com/Elemeants)

## Roadmap

Currently, the project is in its early stages. The following features are planned for future releases:

- [ ] Separate frame/display buffer from the waveshare driver.
- [ ] Add support for microSD card to store images.
- [ ] Implement `Frame` logic to handle rendering images and text on the display.
- [ ] Add support for Wi-Fi connectivity to receive images.
- [ ] Implement basic ditthering logic to display images in B/W format.
