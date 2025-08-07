#pragma once

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include "defs.h"

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

#define WS42_DRIVER_WIDTH 400
#define WS42_DRIVER_HEIGHT 300


typedef struct {
  WORD width;
  WORD height;

  gpio_num_t spi_miso_pin;
  gpio_num_t spi_mosi_pin;
  gpio_num_t spi_clk_pin;
  gpio_num_t spi_cs_pin;

  gpio_num_t gpio_busy_pin;
  gpio_num_t gpio_rst_pin;
  gpio_num_t gpio_dc_pin;

  spi_host_device_t spi_bus;
} WS42_Driver_Config_t;

spi_device_handle_t WS42_Driver_Init(WS42_Driver_Config_t*);
spi_device_interface_config_t WS42_Driver_GetInterfaceConfig(void);

spi_bus_config_t WS42_Driver_getSPIBusConfig(void);
spi_device_interface_config_t WS42_Driver_getSPIDeviceConfig(void);

void WS42_Driver_WaitForBusyAck(void);
void WS42_Driver_HardReset(void);
void WS42_Driver_ClearScreen(void);

void WS42_Driver_BlackScreen(void);

void WS42_Driver_SendCMD(WS42_Driver_CMD_e);
void WS42_Driver_SendDataByte(BYTE);
void WS42_Driver_SendDataBuffer(BYTE*, WORD);

void WS42_Driver_DrawBuffer(
  WORD pos_x,
  WORD pos_y,
  BYTE* buffer,
  WORD buffer_len
);
