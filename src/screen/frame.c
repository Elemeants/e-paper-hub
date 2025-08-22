/**
 * @file frame.c
 * @author jdanypa@gmail.com (Elemeants)
 */
#include "frame.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <esp_log.h>

#include "fonts/fonts.h"
#include "utils/timing.h"

#define HORIZONTAL 0
#define VERTICAL 1

/** Private variables */

static const char* TAG = "frame";

/** Private functions */

static inline int16_t _math_calculate_slope_distances(uint16_t dx,
                                                      uint16_t dy) {
  dx = dx == 0 ? 1 : dx;  // Prevent division by zero
  dy = dy == 0 ? 1 : dy;  // Prevent division by zero
  return dy / dx;
}

static inline int16_t _math_calculate_slope_points(uint16_t x1, uint16_t y1,
                                                   uint16_t x2, uint16_t y2) {
  int16_t dx = abs(x2 - x1);
  int16_t dy = abs(y2 - y1);
  return _math_calculate_slope_distances(dx, dy);
}

static inline uint8_t _ensure_bounds(uint16_t x1, uint16_t y1, uint16_t x2,
                                     uint16_t y2, uint16_t width,
                                     uint16_t height) {
  if (x1 > width || y1 > height || x2 > width || y2 > height) {
    return 0;  // Out of bounds
  }
  return 1;  // Within bounds
}

static inline void _graphics_frame_buffer_draw_bitmap(
    graphics_frame_buffer_t *frame_buffer, uint16_t x, uint16_t y,
    const uint8_t *data, uint16_t width, uint16_t height,
    graphics_color_e color, int invert) {
  for (uint16_t row = 0; row < height; row++) {
    for (uint16_t col = 0; col < width; col++) {
      const uint32_t col_font_idx = (col / 8);
      const uint16_t byte_index = (row * BIT_CAPACITY(width)) + col_font_idx;
      uint8_t font_bitmask = data[byte_index];
      if (invert) {
        font_bitmask = ~font_bitmask;
      }

      const uint8_t bit = (font_bitmask & (1 << (7 - (col % 8)))) != 0;

      if (!bit) {
        continue;  // Skip if the pixel is not set
      }

      graphics_frame_buffer_draw_pixel(frame_buffer, x + col, y + row, color);
    }
  }
}

/** Public functions */

// graphics_frame_buffer_t graphics_frame_buffer_create(uint16_t width,
//                                                      uint16_t height) {
//   graphics_frame_buffer_t frame_buffer;

//   frame_buffer.width = width;
//   frame_buffer.height = height;
//   ESP_LOGI(TAG, "Allocating frame_buffer for (%d, %d) = %d (av = %d)", width, height,
//            width * height, heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));
//   frame_buffer.buffer = (uint8_t *)malloc(width * height);
//   if (frame_buffer.buffer == NULL) {
//     ESP_LOGE(TAG,
//              "Critical error: frame_buffer can't be allocated av=%d, try=%d",
//              heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
//              width * height);
//     return frame_buffer;
//   }

//   memset(frame_buffer.buffer, 0, width * height);

//   return frame_buffer;
// }

void graphics_frame_buffer_destroy(graphics_frame_buffer_t *frame_buffer) {
  if (frame_buffer->buffer) {
    free(frame_buffer->buffer);
    frame_buffer->buffer = NULL;
  }

  frame_buffer->width = 0;
  frame_buffer->height = 0;
}

void graphics_frame_buffer_clear(graphics_frame_buffer_t *frame_buffer,
                                 graphics_color_e color) {
  memset(frame_buffer->buffer, (uint8_t)color,
         frame_buffer->width * frame_buffer->height * sizeof(uint8_t));
}

inline void graphics_frame_buffer_draw_pixel(
    graphics_frame_buffer_t *frame_buffer, uint16_t x, uint16_t y,
    graphics_color_e color) {
  if (x < frame_buffer->width && y < frame_buffer->height) {
    frame_buffer->buffer[(y * frame_buffer->width) + x] = (uint8_t)color;
  }
}

void graphics_frame_buffer_draw_line(graphics_frame_buffer_t *frame_buffer,
                                     uint16_t x1, uint16_t y1, uint16_t x2,
                                     uint16_t y2, graphics_color_e color) {
  if (!_ensure_bounds(x1, y1, x2, y2, frame_buffer->width,
                      frame_buffer->height)) {
    return;  // Out of bounds
  }

  const int16_t distance_x = abs(x2 - x1);
  const int16_t distance_y = abs(y2 - y1);
  const int16_t slope = _math_calculate_slope_distances(distance_x, distance_y);

  int16_t start = x1;
  int16_t end = x2;
  for (uint16_t idx = start; idx < end; idx++) {
    const int16_t f_idx = slope * (idx - start) + y1;
    if (f_idx < 0 || f_idx >= frame_buffer->height) {
      break;
    }

    graphics_frame_buffer_draw_pixel(frame_buffer, idx, f_idx, color);
  }
}

void graphics_frame_buffer_draw_rectangle(graphics_frame_buffer_t *frame_buffer,
                                          uint16_t x, uint16_t y,
                                          uint16_t width, uint16_t height,
                                          graphics_color_e color) {
  graphics_frame_buffer_draw_line(frame_buffer, x, y, x + width, y, color);
  graphics_frame_buffer_draw_line(frame_buffer, x, y, x, y + height, color);
  graphics_frame_buffer_draw_line(frame_buffer, x + width, y, x + width,
                                  y + height, color);
  graphics_frame_buffer_draw_line(frame_buffer, x, y + height, x + width,
                                  y + height, color);
}

void graphics_frame_buffer_fill_rectangle(graphics_frame_buffer_t *frame_buffer,
                                          uint16_t x, uint16_t y,
                                          uint16_t width, uint16_t height,
                                          graphics_color_e color) {
  for (uint16_t i = 0; i < width; i++) {
    for (uint16_t j = 0; j < height; j++) {
      graphics_frame_buffer_draw_pixel(frame_buffer, x + i, y + j, color);
    }
  }
}

void dump_graphics_frame_buffer(const graphics_frame_buffer_t *frame_buffer) {
  printf("Dumping graphics frame buffer:\n");
  for (uint16_t y = 0; y < frame_buffer->height; y++) {
    for (uint16_t x = 0; x < frame_buffer->width; x++) {
      printf("%01X", frame_buffer->buffer[(y * frame_buffer->width) + x]);
    }
    printf("\n");
    sleep_ms(10);  // Sleep to avoid flooding the console
  }
  printf("\n");
}

void graphics_frame_buffer_draw_text(graphics_frame_buffer_t *frame_buffer,
                                     uint16_t x, uint16_t y, const char *text,
                                     graphics_color_e color) {
  uint16_t _x = x;
  for (const char *p = text; *p != '\0'; p++) {
    const uint8_t *data = font16_get_entry(*p);
    _graphics_frame_buffer_draw_bitmap(
        frame_buffer, _x, y, data, font_16.c_width, font_16.c_height, color, 0);
    _x += font_16.c_width;
  }
}

void graphics_frame_buffer_draw_bitmap(graphics_frame_buffer_t *frame_buffer,
                                       uint16_t x, uint16_t y,
                                       const uint8_t *data, uint16_t width,
                                       uint16_t height,
                                       graphics_color_e color) {
  _graphics_frame_buffer_draw_bitmap(frame_buffer, x, y, data, width, height,
                                     color, 1);
}
