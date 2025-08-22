/**
 * @file max17048_i2c_driver.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief MAX17048 I2C driver for battery management
 *
 * This driver provides an interface to communicate with the MAX17048 battery fuel gauge,
 * and exposes functions to initialize the driver and retrieve battery percentage.
 *
 * This header file defines the configuration structure and lists the available registers.
 *
 * Datasheet: https://uelectronics.com/wp-content/uploads/2023/02/AR3450-MAX17048GT10-SMD-DFN-8-EP2x2-Datasheet.pdf
 */
#pragma once

#include <driver/i2c_master.h>
#include <soc/gpio_num.h>
#include "utils/defs.h"

/**
 * @brief Configuration structure for the MAX17048 I2C driver
 *
 * This structure holds the configuration parameters for the MAX17048 I2C driver,
 * including the GPIO pins used for I2C communication.
 */
typedef struct {
  /**
   * @brief GPIO pin used for I2C data (SDA)
   */
  gpio_num_t sda_pin;

  /**
   * @brief GPIO pin used for I2C clock (SCL)
   */
  gpio_num_t scl_pin;
} max17048_i2c_driver_config_t;

/**
 * @brief MAX17048 registers.
 *
 * This enumeration lists the registers available in the MAX17048 device,
 * which can be accessed to read or write battery-related data.
 */
typedef enum {
  MAX17048_REG_VCELL = 0x02,
  MAX17048_REG_SOC = 0x04,
  MAX17048_REG_MODE = 0x06,
  MAX17048_REG_VERSION = 0x08,
  MAX17048_REG_HIBRT = 0x0A,
  MAX17048_REG_CONFIG = 0x0C,
  MAX17048_REG_VALRT = 0x14,
  MAX17048_REG_CRATE = 0x16,
  MAX17048_REG_VRESET_ID = 0x18,
  MAX17048_REG_STATUS = 0x1A,
  MAX17048_REG_TABLE_STARTS = 0x40,
  MAX17048_REG_TABLE_ENDS = 0x7F,
  MAX17048_REG_CMD = 0xFE,
} max17048_i2c_registers_e;

/**
 * @brief Initializes the MAX17048 I2C driver with the specified configuration.
 *
 * This function sets up the I2C bus and prepares the driver for communication with the MAX17048 device.
 *
 * @param dev_config Configuration structure containing GPIO pins for I2C communication.
 */
uint8_t max17048_i2c_driver_init(max17048_i2c_driver_config_t user_config);

/**
 * @brief Retrieves the battery percentage from the MAX17048 device.
 *
 * This function reads the state of charge (SOC) register from the MAX17048 device
 * and calculates the battery percentage based on the raw value.
 *
 * @return uint8_t Returns the battery percentage as a value between 0 and 100.
 */
uint8_t max17048_i2c_driver_get_batt_percent(void);
