/**
 * @file waveshare_42in_spi_driver.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Waveshare 4.2 inch SPI display driver
 *
 * This is a driver for the Waveshare 4.2 inch SPI display it supports both black/white and red color.
 * It is intended to be used with an ESP32 microcontroller, as it utilizes the ESP-IDF framework's SPI and GPIO HAL
 * interfaces.
 *
 * This driver describes the commands exposed at the datasheet, and provides functions to initialize the display.
 *
 * Initialization example:
 * ```c
 * WS42_Driver_Config_t config = {
 *   // Set the width and height according to your display's specifications
 *   .width = 400,
 *   .height = 300,
 *
 *   // Define the GPIO pins used for SPI communication and control signals
 *   .spi_miso_pin = GPIO_NUM_25,
 *   .spi_mosi_pin = GPIO_NUM_23,
 *   .spi_clk_pin = GPIO_NUM_19,
 *   .spi_cs_pin = GPIO_NUM_22,
 *   .gpio_busy_pin = GPIO_NUM_21,
 *   .gpio_rst_pin = GPIO_NUM_18,
 *   .gpio_dc_pin = GPIO_NUM_5,
 *   .spi_bus = HSPI_HOST
 * };
 *
 * ws42_driver_init(config);
 * ```
 *
 * Datasheet: https://www.waveshare.net/w/upload/8/88/UC8176.pdf
 */
#pragma once

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include "utils/defs.h"

/**
 * @brief Command enumeration for the WS42 display driver
 * This enumeration defines the various commands that can be sent to the WS42 display.
 * And was taken from the official datasheet.
 */
typedef enum {
  WS42_Driver_CMD_PANEL_SETTINGS  = 0x00,
  WS42_Driver_CMD_POWER_SETTINGS  = 0x01,
  WS42_Driver_CMD_POWER_OFF       = 0x02,
  WS42_Driver_CMD_POWER_OFF_SEQ   = 0x03,
  WS42_Driver_CMD_POWER_ON        = 0x04,
  WS42_Driver_CMD_POWER_ON_MES    = 0x05,
  WS42_Driver_CMD_BOOSTER_SOFT_ST = 0x06,
  WS42_Driver_CMD_DEEP_SLEEP      = 0x07,
  WS42_Driver_CMD_DATA_BW_START   = 0x10,
  WS42_Driver_CMD_DATA_STOP       = 0x11,
  WS42_Driver_CMD_DISPLAY_REFRESH = 0x12,
  WS42_Driver_CMD_DATA_RED_START  = 0x13,
  WS42_Driver_CMD_PLL_CONTROL     = 0x30,
  WS42_Driver_CMD_TEMP_SENSOR_CAL = 0x40,
  WS42_Driver_CMD_TEMP_SENSOR_SEL = 0x41,
  WS42_Driver_CMD_TEMP_WRITE      = 0x42,
  WS42_Driver_CMD_TEMP_READ       = 0x43,
  WS42_Driver_CMD_VCOM_SET        = 0x50,
  WS42_Driver_CMD_LOW_PWR_DETECT  = 0x51,
  WS42_Driver_CMD_TCON_SETTING    = 0x60,
  WS42_Driver_CMD_RESOLUTION_SET  = 0x61,
  WS42_Driver_CMD_GSST_SETTING    = 0x65,
  WS42_Driver_CMD_GET_REVISION    = 0x70,
  WS42_Driver_CMD_GET_BUSY        = 0x71,
  WS42_Driver_CMD_AUTO_MEASURE_VCOM = 0x80,
  WS42_Driver_CMD_VCOM_VALUE      = 0x81,
  WS42_Driver_CMD_VCOM_DC_SET     = 0x82,
  WS42_Driver_CMD_PARTIAL_WINDOW  = 0x90,
  WS42_Driver_CMD_PARTIAL_IN      = 0x91,
  WS42_Driver_CMD_PARTIAL_OUT     = 0x92,
  WS42_Driver_CMD_PROGRAM_MODE    = 0xA0,
  WS42_Driver_CMD_ACTIVE_PROG     = 0xA1,
  WS42_Driver_CMD_READ_OTP        = 0xA2,
  WS42_Driver_CMD_CASCADE_SETTING = 0xE0,
  WS42_Driver_CMD_POWER_SAVING    = 0xE3,
  WS42_Driver_CMD_LPD_SELECT      = 0xE4,
  WS42_Driver_CMD_FORCE_TEMP      = 0xE5,
} WS42_Driver_CMD_e;

/**
 * @brief Structure that defines the screen dimensions and GPIO pin configuration for the ESP32 SPI interface.
 */
typedef struct {
  /**
   * @brief Width of the display in pixels
   */
  WORD width;

  /**
   * @brief Height of the display in pixels
   */
  WORD height;

  /**
   * @brief GPIO pin connected to the SPI MISO (Master In Slave Out) line
   */
  gpio_num_t spi_miso_pin;

  /**
   * @brief GPIO pin connected to the SPI MOSI (Master Out Slave In) line
   */
  gpio_num_t spi_mosi_pin;

  /**
   * @brief GPIO pin connected to the SPI Clock line
   */
  gpio_num_t spi_clk_pin;

  /**
   * @brief GPIO pin connected to the SPI Chip Select (CS) line
   */
  gpio_num_t spi_cs_pin;

  /**
   * @brief GPIO pin connected to the display busy signal, this signal indicates when the display is busy processing
   * data and should not be sent new commands.
   *
   * GPIO is configured as input, and the busy state is indicated by a low level on this pin.
   */
  gpio_num_t gpio_busy_pin;

  /**
   * @brief GPIO pin connected to the display reset signal, used to reset the display hardware.
   * GPIO is configured as output, and should be driven low to reset the display.
   */
  gpio_num_t gpio_rst_pin;

  /**
   * @brief GPIO pin connected to the display data/command control line, used to switch between sending data and
   * commands to the display.
   * GPIO is configured as output, and should be driven high to send data and low to send commands.
   */
  gpio_num_t gpio_dc_pin;

  /**
   * @brief SPI bus to use for communication with the display, typically SPI2_HOST on the ESP32.
   */
  spi_host_device_t spi_bus;
} WS42_Driver_Config_t;

/**
 * @brief Initializes the WS42 display driver with the specified configuration.
 *
 * This function sets up the SPI bus (See `WS42_Driver_getSPIBusConfig`, and `WS42_Driver_getSPIDeviceConfig`)
 * and configures the GPIO pins according to the provided `WS42_Driver_Config_t` structure.
 *
 * Then it performs a hardware reset of the display, waits for it to become ready, and starts the initialization
 * sequence.
 *
 * @return spi_device_handle_t Returns the SPI device handle.
 */
spi_device_handle_t ws42_driver_init(const WS42_Driver_Config_t);

/**
 * @brief Waits for the display to become ready for the next command.
 *
 * @note This function checks the busy pin of the display and waits until it is low.
 */
void ws42_driver_wait_busy_ack(void);

/**
 * @brief Performs a hardware reset of the display.
 * 
 * @note This function sets the `WS42_Driver_Config_t#gpio_rst_pin` to low for a short period.
 */
void ws42_driver_hard_reset(void);

/**
 * @brief Clears the display by filling it with white color.
 */
void ws42_driver_ops_clear_screen(void);

/**
 * @brief Sends a WS42_Driver_CMD_e command to the display.
 *
 * @note This function sends a command byte to the display using the SPI interface, and sets the DC pin to LOW.
 */
void ws42_driver_send_command(WS42_Driver_CMD_e);

/**
 * @brief Sends a data byte to the display.
 *
 * @note This function sends a data byte to the display using the SPI interface, and sets the DC pin to HIGH.
 */
void ws42_driver_send_data(BYTE);

/**
 * @brief Sends a data buffer to the display.
 *
 * @note This function sends a data buffer to the display using the SPI interface, and sets the DC pin to HIGH.
 */
void ws42_driver_send_data_buffer(BYTE* data, WORD data_length);
