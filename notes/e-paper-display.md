# E-Paper Display Driver Notes

This document provides an overview of the Waveshare 4.2 inch e-Paper display driver, including its features, usage, and resources.

## Developer Notes

We are currently working on the Waveshare 4.2 inch e-Paper display driver, which is based on the UC8176 controller. This display supports a resolution of 400x300 pixels and uses SPI for communication.

There are two versions of the display driver; new and old. This project uses the old version, which has a **completely different API**. The new version is not compatible with the old one, so please ensure you are using the correct version (Or porting your code to the new version).

---

### Initialization sequence

1. Configure the GPIO pins for the display.
   - Set the DC pin to output mode.
   - Set the RST pin to output mode.
   - Set the BUSY pin to input mode.
1. Initialize the SPI bus.
   - Set the SPI clock speed to 1 MHz.
   - Set the SPI bus width to 1 bit. (Single MOSI line)
1. Hardware reset the display.
   - Set the RST pin low for at least 10 ms.
1. Software initialization.

   - Do a software reset by sending the `0x12` command.
   - Wait for busy pin to go low (indicating the display is ready).
   - Configures the Panel settings by sending the `0x00` command with the following data byte `0x0F`:
     | Parameter | Value | Bit     | Description  |
     | --------- | ----- | ------- | ------------ |
     | RES       | `00b` | `[7:6]` | 400x300      |
     | REG_EN    | `0b`  | `[5]`   | LUT FROM OTP |
     | BWR       | `0b`  | `[4]`   | B/W/R Pixel  |
     | UD        | `1b`  | `[3]`   | Scan-up      |
     | SHL       | `1b`  | `[2]`   | Shift-right  |
     | SHD_N     | `1b`  | `[1]`   | Booster ON   |
     | RST_N     | `1b`  | `[0]`   | No effect    |

1. Clear the display buffer.

   - Send the `0x10` command with the data byte `0xFF` to set the display to white.
      > [!NOTE]
      > Uses `0xFF` for black due to how data transmission settings are configured.
      > From `VCOM AND DATA INTERVAL SETTING` documentation, the default data polarity
      > is set as follows:
      >
      > | Data (R, B/W) | Pixel Color |
      > | ------------- | ----------- |
      > | `00b`         | BLACK       |
      > | `01b`         | WHITE       |
      > | `10b`         | RED         |
      > | `11b`         | RED         |
   - Send the `0x13` command with the data byte `0x00` to set the display to none for Red register.
   - Finally, send the `0x12` command to update the display with the new buffer.
   - Wait for the busy pin to go low again.

## Resources

- [Datasheet](https://www.waveshare.net/w/upload/8/88/UC8176.pdf)
- [Waveshare Support Page](<https://www.waveshare.com/wiki/4.2inch_e-Paper_Module_(B)_Manual#Resources>)
