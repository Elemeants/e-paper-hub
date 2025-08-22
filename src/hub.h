#pragma once

#include "drivers/battery/max17048_i2c_driver.h"
#include "drivers/display/waveshare_42in_spi_driver.h"
#include "drivers/sdcard/sd_spi_driver.h"
#include "screen/frame.h"

struct ImageNode {
  char* path;
  struct ImageNode* next;
};
typedef struct ImageNode ImageNode;

typedef struct {
  ws42_driver_config_t screen_config;
  max17048_i2c_driver_config_t battery_reader_config;
  sdcard_driver_config_t sdcard_config;
} e_paper_hub_peripherals_config_t;

typedef struct {
  uint8_t screen_status;
  uint8_t sdcard_status;
  uint8_t batt_sensor_status;
} e_paper_hub_hardware_status_t;

typedef struct {
  e_paper_hub_peripherals_config_t _settings;
  e_paper_hub_hardware_status_t status;

  ImageNode* images;
  graphics_frame_buffer_t frame_buffer;
  uint8_t battery_level;
} e_paper_hub_system_stats_t;

extern e_paper_hub_system_stats_t e_paper_hub_dev;

void hub_initialize(const e_paper_hub_peripherals_config_t settings);
void hub_render_next_image(void);
