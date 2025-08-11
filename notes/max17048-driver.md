# MAX17048 Driver Notes

This document provides an overview of the MAX17048 battery fuel gauge driver, including its features, usage, and resources.

## Developer Notes

We are currently working on the MAX17048 battery fuel gauge driver, which is based on the I2C communication protocol.

### Initialization sequence

There is not much initialization required for the MAX17048. The driver is designed to be simple and straightforward, so we can initialize our I2C bus and the 

1. Configure the I2C bus.
   - Set the I2C clock speed to 100 kHz.
   - Set the device address to `0x36` (default for MAX17048).
1. Initialize the I2C device.

### Reading Battery Level

1. Read the battery level by accessing the `0x04` register.
1. Convert the raw value to a percentage.
   - The raw value is a 8-bit unsigned integer representing the battery voltage as `1%/256`.
   - The percentage can be calculated using the formula: `percentage = (raw_value * 100 / 256)`.
   > We want to multiply by 100 first to avoid float operations.

## Resources

- [MAX17048 Datasheet](https://uelectronics.com/wp-content/uploads/2023/02/AR3450-MAX17048GT10-SMD-DFN-8-EP2x2-Datasheet.pdf)
