/**
 * @file main.c
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Main application file
 *
 * This file contains the main application logic for the InkPortrait project.
 * It initializes the Waveshare 4.2 inch SPI display and the MAX17048 battery
 * driver.
 */
#include <stdio.h>

#include "drivers/battery/max17048_i2c_driver.h"
#include "drivers/display/waveshare_42in_spi_driver.h"
#include "drivers/sdcard/sd_spi_driver.h"
#include "screen/renderer.h"
#include "utils/timing.h"
#include "test_image.h"
#include "esp_log.h"
#include "hub.h"

static const e_paper_hub_peripherals_config_t global_device_configs = {
  .screen_config = {
    .height = SCREEN_HEIGHT,
    .width = SCREEN_WIDTH,
    .spi_clk_pin = GPIO_NUM_19,
    .spi_mosi_pin = GPIO_NUM_23,
    .spi_miso_pin = GPIO_NUM_25,
    .spi_cs_pin = GPIO_NUM_22,
    .gpio_busy_pin = GPIO_NUM_25,
    .gpio_rst_pin = GPIO_NUM_26,
    .gpio_dc_pin = GPIO_NUM_27,
    .spi_bus = SPI2_HOST,
  },
  .sdcard_config = {
    .host = SPI3_HOST,
    .clk_pin = GPIO_NUM_5,
    .mosi_pin = GPIO_NUM_17,
    .miso_pin = GPIO_NUM_16,
    .cs_pin = GPIO_NUM_18,
  },
  .battery_reader_config = {
    .sda_pin = GPIO_NUM_14,
    .scl_pin = GPIO_NUM_13,
  }
};


void app_main() {
  hub_initialize(global_device_configs);

  printf("Battery Percentage: %d%%\n", max17048_i2c_driver_get_batt_percent());

  while (1) {
    hub_render_next_image();
    sleep_ms(10000);
  }
}
