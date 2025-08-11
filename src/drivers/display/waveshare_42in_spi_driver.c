/**
 * @file waveshare_42in_spi_driver.c
 * @author jdanypa@gmail.com (Elemeants)
 */
#include "waveshare_42in_spi_driver.h"

#include <esp_attr.h>
#include <stdio.h>
#include <string.h>

#include "utils/timing.h"

#define WAIT_BUSY 1
#define NOT_WAIT_BUSY 0
#define NOTHING 0x00

/** Init sequence helpers */

typedef struct {
  BYTE cmd;
  BYTE data_len;
  BYTE wait_busy;

  BYTE data[16];
} ws42_driver_init_table_entry_t;

DRAM_ATTR static const ws42_driver_init_table_entry_t _ws42_driver_init_seq[] =
    {
        {WS42_Driver_CMD_POWER_ON, 0, WAIT_BUSY, {NOTHING}},
        {WS42_Driver_CMD_PANEL_SETTINGS, 1, NOT_WAIT_BUSY, {0x0F}},
};

/** Private variables */

static WS42_Driver_Config_t driver_config;
static spi_device_handle_t screen_spi_handler;

/** Private function declarations */

static spi_bus_config_t ws42_driver_get_spi_bus_config(void) {
  spi_bus_config_t buscfg = {.miso_io_num = driver_config.spi_miso_pin,
                             .mosi_io_num = driver_config.spi_mosi_pin,
                             .sclk_io_num = driver_config.spi_clk_pin,
                             .quadwp_io_num = -1,
                             .quadhd_io_num = -1,
                             .flags = SPICOMMON_BUSFLAG_SLAVE};
  return buscfg;
}

static void IRAM_ATTR __handle_data_command_pre_transmision(spi_transaction_t* trans) {
  int dc = (int)trans->user;
  gpio_set_level(driver_config.gpio_dc_pin, dc);
}

static spi_device_interface_config_t ws42_driver_get_spi_device_config(void) {
  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = _MHZ(1),
      .mode = 0,
      .spics_io_num = driver_config.spi_cs_pin,
      .queue_size = 12,
      .pre_cb = __handle_data_command_pre_transmision,
  };
  return devcfg;
}

static void _ws42_driver_init_gpio(void) {
  gpio_config_t pin;
  pin.intr_type = GPIO_INTR_DISABLE, pin.mode = GPIO_MODE_OUTPUT,
  pin.pull_up_en = ENABLE, pin.pin_bit_mask = _BIT(driver_config.gpio_rst_pin),
  gpio_config(&pin);

  pin.intr_type = GPIO_INTR_DISABLE, pin.mode = GPIO_MODE_OUTPUT,
  pin.pin_bit_mask = _BIT(driver_config.gpio_dc_pin), gpio_config(&pin);

  pin.intr_type = GPIO_INTR_DISABLE;
  pin.mode = GPIO_MODE_INPUT;
  pin.pull_down_en = DISABLE;
  pin.pull_up_en = DISABLE;
  pin.pin_bit_mask = _BIT(driver_config.gpio_busy_pin), gpio_config(&pin);

  gpio_set_level(driver_config.gpio_rst_pin, 1);
  gpio_set_level(driver_config.gpio_dc_pin, 0);
}

static void _ws42_driver_exec_init_table(void) {
  for (BYTE tb_idx = 0; tb_idx < ARRAY_SIZE(_ws42_driver_init_seq); tb_idx++) {
    const ws42_driver_init_table_entry_t entry = _ws42_driver_init_seq[tb_idx];

    ws42_driver_send_command((WS42_Driver_CMD_e)entry.cmd);

    for (BYTE data_idx = 0; data_idx < entry.data_len; data_idx++) {
      ws42_driver_send_data(entry.data[data_idx]);
    }

    if (entry.wait_busy) {
      ws42_driver_wait_busy_ack();
    }
  }

  ws42_driver_wait_busy_ack();
}

static void _ws42_driver_spi_send_byte(const BYTE data, const BYTE data_command,
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

static void _ws42_driver_init_spi_device(void) {
  const spi_bus_config_t buscfg = ws42_driver_get_spi_bus_config();
  const spi_device_interface_config_t devcfg =
      ws42_driver_get_spi_device_config();
  spi_device_handle_t spi;
  esp_err_t ret;

  ret = spi_bus_initialize(driver_config.spi_bus, &buscfg, SPI_DMA_DISABLED);
  ESP_ERROR_CHECK(ret);
  ret = spi_bus_add_device(driver_config.spi_bus, &devcfg, &spi);
  ESP_ERROR_CHECK(ret);
  screen_spi_handler = spi;
}

/** Public function declarations */

spi_device_handle_t ws42_driver_init(const WS42_Driver_Config_t config) {
  driver_config = config;

  _ws42_driver_init_gpio();
  _ws42_driver_init_spi_device();
  ws42_driver_hard_reset();
  sleep_ms(1000);
  _ws42_driver_exec_init_table();
  ws42_driver_ops_clear_screen();

  return screen_spi_handler;
}

void ws42_driver_ops_clear_screen(void) {
  WORD Width;
  WORD Height;

  Width = (driver_config.width % 8 == 0) ? (driver_config.width / 8)
                                         : (driver_config.width / 8 + 1);
  Height = driver_config.height;

  ws42_driver_send_command(WS42_Driver_CMD_DATA_BW_START);
  for (WORD j = 0; j < Height; j++) {
    for (WORD i = 0; i < Width; i++) {
      ws42_driver_send_data(0xFF);
    }
  }
  ws42_driver_send_command(WS42_Driver_CMD_DATA_RED_START);
  for (WORD j = 0; j < Height; j++) {
    for (WORD i = 0; i < Width; i++) {
      ws42_driver_send_data(0x00);
    }
  }

  ws42_driver_send_command(WS42_Driver_CMD_DISPLAY_REFRESH);
  sleep_ms(200);
  ws42_driver_wait_busy_ack();
}

void ws42_driver_hard_reset(void) {
  gpio_set_level(driver_config.gpio_rst_pin, 1);
  sleep_ms(200);
  gpio_set_level(driver_config.gpio_rst_pin, 0);
  sleep_ms(10);
  gpio_set_level(driver_config.gpio_rst_pin, 1);
  sleep_ms(200);
  ws42_driver_wait_busy_ack();
}

void ws42_driver_wait_busy_ack() {
  do {
    ws42_driver_send_command(WS42_Driver_CMD_GET_BUSY);
    sleep_ms(20);
  } while (!(gpio_get_level(driver_config.gpio_busy_pin)));
  sleep_ms(20);
}

void ws42_driver_send_command(WS42_Driver_CMD_e cmd) {
  _ws42_driver_spi_send_byte((const BYTE)cmd, 0, false);
}

void ws42_driver_send_data(BYTE data) {
  _ws42_driver_spi_send_byte(data, 1, false);
}
