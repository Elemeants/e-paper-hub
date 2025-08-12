/**
 * @file renderer.c
 * @author jdanypa@gmail.com (Elemeants)
 */
#include "renderer.h"

#include <string.h>

#include "drivers/display/waveshare_42in_spi_driver.h"
#include "utils/timing.h"

/** Private variables */

static graphics_frame_buffer_t *frame_buffer = NULL;

/** Private functions */

static void _copy_buffer_to_spi_data(BYTE *data_buffer, WORD width, WORD height, BYTE mask) {
  memset(data_buffer, 0x00, width * height);

  for (WORD j = 0; j < frame_buffer->height; j++) {
    for (WORD i = 0; i < frame_buffer->width; i++) {
      WORD dst_idx = j * ((frame_buffer->width + 7) / 8) + (i / 8);
      BYTE bit_mask = 1 << (7 - (i % 8));  // MSB-first for most displays

      if (frame_buffer->buffer[j * frame_buffer->width + i] & mask) {
        data_buffer[dst_idx] |= bit_mask;  // Set bit
      }
    }
  }
}

static void _copy_buffer_to_spi_data_black_reg(BYTE *data_buffer, WORD width,
                                               WORD height) {
  _copy_buffer_to_spi_data(data_buffer, width, height, 0x01);
}

static void _copy_buffer_to_spi_data_red_reg(BYTE *data_buffer, WORD width,
                                             WORD height) {
  _copy_buffer_to_spi_data(data_buffer, width, height, 0x02);
}

void _dump_graphics_frame_buffer(BYTE *data_buffer, WORD width, WORD height) {
  printf("Dumping graphics frame buffer:\n");
  for (WORD y = 0; y < height; y++) {
    for (WORD x = 0; x < width; x++) {
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

void graphics_renderer_detach(void) { frame_buffer = NULL; }

void graphics_renderer_update(void) {
  if (frame_buffer == NULL) {
    return;  // No frame buffer attached
  }

  // Render the frame buffer to the display
  WORD width = (frame_buffer->width % 8 == 0) ? (frame_buffer->width / 8)
                                              : (frame_buffer->width / 8 + 1);
  WORD height = frame_buffer->height;
  BYTE *data_buffer = (BYTE *)malloc(width * height);

  _copy_buffer_to_spi_data_black_reg(data_buffer, width, height);
  ws42_driver_send_command(WS42_Driver_CMD_DATA_BW_START);
  ws42_driver_send_data_buffer(data_buffer, width * height);

  _copy_buffer_to_spi_data_red_reg(data_buffer, width, height);
  ws42_driver_send_command(WS42_Driver_CMD_DATA_RED_START);
  ws42_driver_send_data_buffer(data_buffer, width * height);

  free(data_buffer);
  ws42_driver_send_command(WS42_Driver_CMD_DISPLAY_REFRESH);
  sleep_ms(200);
  ws42_driver_wait_busy_ack();
  sleep_ms(2000);
}
