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

  ws42_driver_init(SCREEN_CONFIG);
  max17048_i2c_driver_init(MAX17048_CONFIG);

  printf("Battery Percentage: %d%%\n", max17048_i2c_driver_get_batt_percent());

  sleep_ms(2000);

  printf("Black/Red Screen\n");
  WORD Width;

  Width = (SCREEN_CONFIG.width % 8 == 0)? (SCREEN_CONFIG.width / 8 ): (SCREEN_CONFIG.width / 8 + 1);

  ws42_driver_send_command(WS42_Driver_CMD_DATA_BW_START);
  for (WORD j = 0; j < 3; j++) {
      for (WORD i = 0; i < Width; i++) {
          ws42_driver_send_data(0x0F);
      }
  }
  ws42_driver_send_command(WS42_Driver_CMD_DATA_RED_START);
  for (WORD j = 0; j < 3; j++) {
      for (WORD i = 0; i < Width; i++) {
          ws42_driver_send_data(0x00);
      }
  }
  for (WORD j = 3; j < 6; j++) {
      for (WORD i = 0; i < Width; i++) {
          ws42_driver_send_data(0xF0);
      }
  }

  ws42_driver_send_command(WS42_Driver_CMD_DISPLAY_REFRESH);
  sleep_ms(200);
  ws42_driver_wait_busy_ack();
  sleep_ms(2000);

  printf("Finished\n");
  while(1) {
    sleep_ms(100);
  }
}
