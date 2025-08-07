#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "waveshare_42in_spi_driver.h"
#include "timing.h"

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

void app_main() {
  WS42_Driver_Init(&SCREEN_CONFIG);

  WAIT_MS(2000);

  printf("Black/Red Screen\n");
  WORD Width;
  WORD Height;

  Width = (SCREEN_CONFIG.width % 8 == 0)? (SCREEN_CONFIG.width / 8 ): (SCREEN_CONFIG.width / 8 + 1);
  Height = SCREEN_CONFIG.height;

  WS42_Driver_SendCMD(WS42_Driver_CMD_DATA_BW_START);
  for (WORD j = 0; j < 3; j++) {
      for (WORD i = 0; i < Width; i++) {
          WS42_Driver_SendDataByte(0x0F);
      }
  }
  WS42_Driver_SendCMD(WS42_Driver_CMD_DATA_RED_START);
  for (WORD j = 0; j < 3; j++) {
      for (WORD i = 0; i < Width; i++) {
          WS42_Driver_SendDataByte(0x00);
      }
  }
  for (WORD j = 3; j < 6; j++) {
      for (WORD i = 0; i < Width; i++) {
          WS42_Driver_SendDataByte(0xF0);
      }
  }

  WS42_Driver_SendCMD(WS42_Driver_CMD_DISPLAY_REFRESH);
  WAIT_MS(200);
  WS42_Driver_WaitForBusyAck();
  WAIT_MS(2000);

  printf("Finished\n");
  while(1) {
    WAIT_MS(100);
  }
}
