/**
 * @file renderer.c
 * @author jdanypa@gmail.com (Elemeants)
 */
#include "renderer.h"

#include <esp_log.h>
#include <string.h>

#include "drivers/display/waveshare_42in_spi_driver.h"
#include "utils/timing.h"

/** Private variables */

static const uint16_t internal_width =
    (SCREEN_WIDTH % 8 == 0) ? (SCREEN_WIDTH / 8) : (SCREEN_WIDTH / 8 + 1);
static const uint16_t internal_height = SCREEN_HEIGHT;
static uint8_t renderer_buffer[CALC_INTERNAL_HEIGH*CALC_INTERNAL_WIDTH];

static graphics_frame_buffer_t *frame_buffer = NULL;

/** Private functions */

static void _copy_buffer_to_spi_data(uint8_t *data_buffer, uint16_t width,
                                     uint16_t height, uint8_t mask) {
  memset(data_buffer, 0x00, width * height);

  for (uint16_t j = 0; j < frame_buffer->height; j++) {
    for (uint16_t i = 0; i < frame_buffer->width; i++) {
      uint16_t dst_idx = j * ((frame_buffer->width + 7) / 8) + (i / 8);
      uint8_t bit_mask = 1 << (7 - (i % 8));  // MSB-first for most displays

      if (frame_buffer->buffer[j * frame_buffer->width + i] & mask) {
        data_buffer[dst_idx] |= bit_mask;  // Set bit
      }
    }
  }
}

static void _copy_buffer_to_spi_data_black_reg(uint8_t *data_buffer,
                                               uint16_t width,
                                               uint16_t height) {
  _copy_buffer_to_spi_data(data_buffer, width, height, 0x01);
}

static void _copy_buffer_to_spi_data_red_reg(uint8_t *data_buffer,
                                             uint16_t width, uint16_t height) {
  _copy_buffer_to_spi_data(data_buffer, width, height, 0x02);
}

void _dump_graphics_frame_buffer(uint8_t *data_buffer, uint16_t width,
                                 uint16_t height) {
  printf("Dumping graphics frame buffer:\n");
  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      printf("%02X ", data_buffer[(y * width) + x]);
    }
    printf("\n");
    sleep_ms(10);  // Sleep to avoid flooding the console
  }
  printf("\n");
}

/** Public functions */

void graphics_renderer_attach(graphics_frame_buffer_t *fb) {
  frame_buffer = fb;
}

void graphics_renderer_detach(void) {
  frame_buffer = NULL;
}

void graphics_renderer_update(void) {
  if (frame_buffer == NULL) {
    return;  // No frame buffer attached
  }

  _copy_buffer_to_spi_data_black_reg(renderer_buffer, internal_width,
                                     internal_height);
  ws42_driver_send_command(WS42_Driver_CMD_DATA_BW_START);
  ws42_driver_send_data_buffer(renderer_buffer,
                               internal_width * internal_height);

  _copy_buffer_to_spi_data_red_reg(renderer_buffer, internal_width,
                                   internal_height);
  ws42_driver_send_command(WS42_Driver_CMD_DATA_RED_START);
  ws42_driver_send_data_buffer(renderer_buffer,
                               internal_width * internal_height);

  ws42_driver_send_command(WS42_Driver_CMD_DISPLAY_REFRESH);
  sleep_ms(200);
  ws42_driver_wait_busy_ack();
  sleep_ms(2000);
}
