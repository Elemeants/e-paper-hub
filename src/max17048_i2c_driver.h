#pragma once

#include <driver/i2c_master.h>
#include <soc/gpio_num.h>
#include <defs.h>

typedef struct {
  gpio_num_t sda_pin;
  gpio_num_t scl_pin;
} max17048_i2c_driver_config_t;

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

void max17048_i2c_driver_init(max17048_i2c_driver_config_t user_config);
BYTE max17048_i2c_driver_get_batt_percent(void);
