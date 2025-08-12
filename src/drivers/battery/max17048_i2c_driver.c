/**
 * @file max17048_i2c_driver.c
 * @author jdanypa@gmail.com (Elemeants)
 */
#include "max17048_i2c_driver.h"
#include "utils/timing.h"

/**
 * @brief I2C device configuration for MAX17048
 */
static const i2c_device_config_t device_config = {
  .dev_addr_length = I2C_ADDR_BIT_7,
  .device_address = 0x36,     // Default I2C address for MAX17048.
  .scl_speed_hz = _KHZ(100),  // 100 kHz I2C speed
};

/** Private variables */

static i2c_master_dev_handle_t dev_handler;
static i2c_master_bus_handle_t bus_handler;
static uint8_t init_status = 0x1;

/** Public function declarations */

void max17048_i2c_driver_init(max17048_i2c_driver_config_t dev_config) {
  esp_err_t err;
  i2c_master_bus_config_t bus_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = dev_config.sda_pin,
    .scl_io_num = dev_config.scl_pin,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .flags = {
      .enable_internal_pullup = 1
    }
  };

  err = i2c_new_master_bus(&bus_config, &bus_handler);
  ESP_ERROR_CHECK(err);
  err = i2c_master_bus_add_device(bus_handler, &device_config, &dev_handler);
  ESP_ERROR_CHECK(err);

  err = i2c_master_probe(bus_handler, device_config.device_address, 2000);
  if (err != ESP_OK) {
    init_status = 0;
    printf("MAX17048 not found at address 0x%02X, disabling this feature!\n", device_config.device_address);
    return;
  }
}

BYTE max17048_i2c_driver_get_batt_percent(void) {
  esp_err_t err;
  BYTE raw_percent = 0;
  BYTE cmd = MAX17048_REG_SOC;

  if (init_status == 0) {
    printf("MAX17048 not initialized, cannot get battery percentage.\n");
    return 0;
  }

  err = i2c_master_transmit_receive(dev_handler, &cmd, 1, &raw_percent, 1, 1000);
  ESP_ERROR_CHECK(err);

  /**
   * From the datasheet raw_percent will be in the dimension of `1%/256` which means
   * 0 -> 0%
   * 128 -> 50%
   * 256 -> 100%
   *
   * First we will need to multiply by 100, and then divide by 256.
   */
  BYTE bat_percent = ((WORD)raw_percent * 100) / 256;
  return bat_percent;
}
