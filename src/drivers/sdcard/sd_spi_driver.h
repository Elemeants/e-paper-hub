/**
 * @file sd_spi_driver.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Initializes the SD card SPI driver with the specified configuration.
 *
 * This function sets up the SPI interface and configures the necessary GPIO pins
 * for communication with the SD card using the provided configuration parameters.
 */
#pragma once

#include <driver/gpio.h>
#include <driver/spi_master.h>

/**
 * @brief Configuration structure for the SD card SPI driver.
 *
 * This structure holds the configuration parameters required to initialize
 * and operate the SD card over SPI interface.
 */
typedef struct {
    spi_host_device_t host;  /*!< Pointer to the SD card host */
    gpio_num_t clk_pin;  /*!< Clock pin for the SD card */
    gpio_num_t mosi_pin; /*!< Master Out Slave In pin for the SD card */
    gpio_num_t miso_pin; /*!< Master In Slave Out pin for the SD card */
    gpio_num_t cs_pin;   /*!< Chip select pin for the SD card */
} sdcard_driver_config_t;

extern const char* SD_VFS_MOUNTING_POINT;

/**
 * @brief Initializes the SD card driver with the specified configuration.
 *
 * This function sets up the SD card driver using the provided configuration
 * parameters. It should be called before any other SD card operations.
 *
 * @param config The configuration structure containing initialization parameters for the SD card driver.
 */
uint8_t sdcard_driver_init(const sdcard_driver_config_t config);
