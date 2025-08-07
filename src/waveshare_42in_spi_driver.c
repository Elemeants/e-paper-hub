#include "waveshare_42in_spi_driver.h"

#include <esp_attr.h>
#include <stdio.h>
#include <string.h>

#include "timing.h"

typedef struct {
  BYTE cmd;
  BYTE data_len;
  BYTE wait_busy;

  BYTE data[16];
} WS42_Driver_Init_CMD_Table_t;

#define WAIT_BUSY 1
#define NOT_WAIT_BUSY 0

DRAM_ATTR static const WS42_Driver_Init_CMD_Table_t INIT_TABLE[] = {
    {WS42_Driver_CMD_POWER_ON, 0, WAIT_BUSY, {0x00}},
    {WS42_Driver_CMD_PANEL_SETTINGS, 1, NOT_WAIT_BUSY, {0x0F}},
};

static WS42_Driver_Config_t* driver_config = NULL;
static spi_device_handle_t   screen_spi_handler;

void _WS42_Driver_InitGPIOs(void);
void _WS42_Driver_ExecInitTable(void);
void _WS42_Driver_InitSPIDevice(void);
void _WS42_Driver_SPI_SendByte(const BYTE data, const BYTE data_command,
                               bool keep_cs_active);

spi_device_handle_t WS42_Driver_Init(WS42_Driver_Config_t* config) {
  driver_config = config;

  _WS42_Driver_InitGPIOs();
  _WS42_Driver_InitSPIDevice();
  WS42_Driver_HardReset();
  WAIT_MS(1000);
  _WS42_Driver_ExecInitTable();
  WS42_Driver_ClearScreen();

  return screen_spi_handler;
}

void WS42_Driver_ClearScreen(void) {
  WORD Width;
  WORD Height;

  Width = (driver_config->width % 8 == 0)? (driver_config->width / 8 ): (driver_config->width / 8 + 1);
  Height = driver_config->height;

  WS42_Driver_SendCMD(WS42_Driver_CMD_DATA_BW_START);
  for (WORD j = 0; j < Height; j++) {
      for (WORD i = 0; i < Width; i++) {
          WS42_Driver_SendDataByte(0xFF);
      }
  }
  WS42_Driver_SendCMD(WS42_Driver_CMD_DATA_RED_START);
  for (WORD j = 0; j < Height; j++) {
      for (WORD i = 0; i < Width; i++) {
          WS42_Driver_SendDataByte(0x00);
      }
  }

  WS42_Driver_SendCMD(WS42_Driver_CMD_DISPLAY_REFRESH);
  WAIT_MS(200);
  WS42_Driver_WaitForBusyAck();
}

void WS42_Driver_HardReset(void) {
  gpio_set_level(driver_config->gpio_rst_pin, 1);
  WAIT_MS(200);
  gpio_set_level(driver_config->gpio_rst_pin, 0);
  WAIT_MS(10);
  gpio_set_level(driver_config->gpio_rst_pin, 1);
  WAIT_MS(200);
  WS42_Driver_WaitForBusyAck();
}

void WS42_Driver_WaitForBusyAck() {
  do {
    WS42_Driver_SendCMD(WS42_Driver_CMD_GET_BUSY);
    WAIT_MS(20);
  } while(!(gpio_get_level(driver_config->gpio_busy_pin)));
  WAIT_MS(20);
}

void _WS42_Driver_InitGPIOs(void) {
  if (!driver_config) {
    return;
  }

  gpio_config_t pin;
  pin.intr_type = GPIO_INTR_DISABLE, pin.mode = GPIO_MODE_OUTPUT,
  pin.pull_up_en = ENABLE, pin.pin_bit_mask = _BIT(driver_config->gpio_rst_pin),
  gpio_config(&pin);

  pin.intr_type = GPIO_INTR_DISABLE, pin.mode = GPIO_MODE_OUTPUT,
  pin.pin_bit_mask = _BIT(driver_config->gpio_dc_pin), gpio_config(&pin);

  pin.intr_type = GPIO_INTR_DISABLE;
  pin.mode = GPIO_MODE_INPUT;
  pin.pull_down_en = DISABLE;
  pin.pull_up_en = DISABLE;
  pin.pin_bit_mask = _BIT(driver_config->gpio_busy_pin), gpio_config(&pin);

  gpio_set_level(driver_config->gpio_rst_pin, 1);
  gpio_set_level(driver_config->gpio_dc_pin, 0);
}

void _WS42_Driver_ExecInitTable(void) {
  for (BYTE tb_idx = 0; tb_idx < ARRAY_SIZE(INIT_TABLE); tb_idx++) {
    const WS42_Driver_Init_CMD_Table_t entry = INIT_TABLE[tb_idx];

    WS42_Driver_SendCMD((WS42_Driver_CMD_e)entry.cmd);

    for (BYTE data_idx = 0; data_idx < entry.data_len; data_idx++) {
      WS42_Driver_SendDataByte(entry.data[data_idx]);
    }

    if (entry.wait_busy) {
      WS42_Driver_WaitForBusyAck();
    }
  }

  WS42_Driver_WaitForBusyAck();
}

void _WS42_Driver_SPI_SendByte(const BYTE data, const BYTE data_command,
                               bool keep_cs_active) {
  esp_err_t error;
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));

  t.length = BYTE_BITS;
  t.tx_buffer = &data;
  t.user = (void*)data_command;
  if (keep_cs_active) {
    t.flags = SPI_TRANS_CS_KEEP_ACTIVE;
  }

  error = spi_device_polling_transmit(screen_spi_handler, &t);
  assert(error == ESP_OK);
}

void WS42_Driver_SendCMD(WS42_Driver_CMD_e cmd) {
  _WS42_Driver_SPI_SendByte((const BYTE)cmd, 0, false);
}

void WS42_Driver_SendDataByte(BYTE data) {
  _WS42_Driver_SPI_SendByte(data, 1, false);
}

void WS42_Driver_SendDataBuffer(BYTE* data, WORD data_len) {
  esp_err_t error;
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));

  t.length = BYTE_BITS * data_len;
  t.tx_buffer = data;
  t.user = (void*)1;
  error = spi_device_polling_transmit(screen_spi_handler, &t);
  assert(error == ESP_OK);
}

void WS42_Driver_DrawBuffer(WORD pos_x, WORD pos_y, BYTE* buffer,
                            WORD buffer_len) {
  
}

spi_bus_config_t WS42_Driver_getSPIBusConfig(void) {
  spi_bus_config_t buscfg = {.miso_io_num = driver_config->spi_miso_pin,
                             .mosi_io_num = driver_config->spi_mosi_pin,
                             .sclk_io_num = driver_config->spi_clk_pin,
                             .quadwp_io_num = -1,
                             .quadhd_io_num = -1,
                             .flags = SPICOMMON_BUSFLAG_SLAVE};
  return buscfg;
}

void IRAM_ATTR __handle_data_command_pre_transmision(spi_transaction_t* trans) {
  int dc = (int)trans->user;
  gpio_set_level(driver_config->gpio_dc_pin, dc);
}

spi_device_interface_config_t WS42_Driver_getSPIDeviceConfig(void) {
  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = _MHZ(1),
      .mode = 0,
      .spics_io_num = driver_config->spi_cs_pin,
      .queue_size = 12,
      .pre_cb = __handle_data_command_pre_transmision,
  };
  return devcfg;
}

void _WS42_Driver_InitSPIDevice(void) {
  const spi_bus_config_t buscfg = WS42_Driver_getSPIBusConfig();
  const spi_device_interface_config_t devcfg = WS42_Driver_getSPIDeviceConfig();
  spi_device_handle_t spi;
  esp_err_t ret;

  ret = spi_bus_initialize(driver_config->spi_bus, &buscfg, SPI_DMA_DISABLED);
  ESP_ERROR_CHECK(ret);
  ret = spi_bus_add_device(driver_config->spi_bus, &devcfg, &spi);
  ESP_ERROR_CHECK(ret);
  screen_spi_handler = spi;
}
