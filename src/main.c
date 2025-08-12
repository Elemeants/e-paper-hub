/**
 * @file main.c
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Main application file
 *
 * This file contains the main application logic for the InkPortrait project.
 * It initializes the Waveshare 4.2 inch SPI display and the MAX17048 battery driver.
 */
#include <stdio.h>
#include "drivers/display/waveshare_42in_spi_driver.h"
#include "drivers/battery/max17048_i2c_driver.h"
#include "utils/timing.h"
#include "screen/renderer.h"

static WS42_Driver_Config_t SCREEN_CONFIG = {
  .height = 300,
  .width  = 400,
  .spi_clk_pin = GPIO_NUM_19,
  .spi_mosi_pin = GPIO_NUM_23,
  .spi_miso_pin = GPIO_NUM_25,
  .spi_cs_pin   = GPIO_NUM_22,
  .gpio_busy_pin = GPIO_NUM_25,
  .gpio_rst_pin = GPIO_NUM_26,
  .gpio_dc_pin = GPIO_NUM_27,
  .spi_bus = SPI2_HOST,
};

static max17048_i2c_driver_config_t MAX17048_CONFIG = {
  .sda_pin = GPIO_NUM_14,
  .scl_pin = GPIO_NUM_13,
};

static char *LOGO =
"  _____                      _    _       _     \n"
"  |  __ \\                    | |  | |     | |    \n"
"  | |__) |_ _ _ __   ___ _ __| |__| |_   _| |__  \n"
"  |  ___/ _` | '_ \\ / _ \\ '__|  __  | | | | '_ \\ \n"
"  | |  | (_| | |_) |  __/ |  | |  | | |_| | |_) |\n"
"  |_|   \\__,_| .__/ \\___|_|  |_|  |_|\\__,_|_.__/ \n"
"              | |                                \n"
"              |_|                                \n";

void app_main() {
  printf(LOGO);
  printf(" Firmware Version: %s\n", FIRMWARE_VERSION);
  printf(" Build time: %u\n", BUILD_TIME_UNIX);

  // Driver inits
  max17048_i2c_driver_init(MAX17048_CONFIG);
  ws42_driver_init(SCREEN_CONFIG);

  // Frame buffer and renderer init
  graphics_frame_buffer_t frame_buffer = graphics_frame_buffer_create(SCREEN_CONFIG.width, SCREEN_CONFIG.height);
  graphics_renderer_attach(&frame_buffer);
  graphics_frame_buffer_clear(&frame_buffer, GRAPHICS_COLOR_WHITE);

  // Do something
  printf("Battery Percentage: %d%%\n", max17048_i2c_driver_get_batt_percent());
  printf("Black/Red Screen\n");
  const WORD step = frame_buffer.height / 3;
  graphics_frame_buffer_fill_rectangle(&frame_buffer, 0, 0, frame_buffer.width, step, GRAPHICS_COLOR_BLACK);
  graphics_frame_buffer_fill_rectangle(&frame_buffer, 0, step, frame_buffer.width, frame_buffer.height - step, GRAPHICS_COLOR_WHITE);
  graphics_frame_buffer_fill_rectangle(&frame_buffer, 0, frame_buffer.height - step, frame_buffer.width, frame_buffer.height, GRAPHICS_COLOR_RED);

  graphics_renderer_update();

  while(1) {
    sleep_ms(100);
  }
}
