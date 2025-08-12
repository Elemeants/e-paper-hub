/**
 * @file frame.c
 * @author jdanypa@gmail.com (Elemeants)
 */
#include "frame.h"

#include "utils/timing.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define HORIZONTAL 0
#define VERTICAL 1

/** Private functions */

static inline int16_t _math_calculate_slope_distances(WORD dx, WORD dy) {
  dx = dx == 0 ? 1 : dx;  // Prevent division by zero
  dy = dy == 0 ? 1 : dy;  // Prevent division by zero
  return dy / dx;
}

static inline int16_t _math_calculate_slope_points(WORD x1, WORD y1, WORD x2,
                                                   WORD y2) {
  int16_t dx = abs(x2 - x1);
  int16_t dy = abs(y2 - y1);
  return _math_calculate_slope_distances(dx, dy);
}

static inline uint8_t _ensure_bounds(WORD x1, WORD y1, WORD x2, WORD y2,
                                     WORD width, WORD height) {
  if (x1 >= width || y1 >= height || x2 >= width || y2 >= height) {
    return 0;  // Out of bounds
  }
  return 1;  // Within bounds
}

/** Public functions */

graphics_frame_buffer_t graphics_frame_buffer_create(WORD width, WORD height) {
  graphics_frame_buffer_t frame_buffer;

  frame_buffer.width = width;
  frame_buffer.height = height;
  frame_buffer.buffer = (BYTE *)malloc(width * height * sizeof(BYTE));
  memset(frame_buffer.buffer, 0, width * height * sizeof(BYTE));

  return frame_buffer;
}

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
  memset(frame_buffer->buffer, (BYTE)color,
         frame_buffer->width * frame_buffer->height * sizeof(BYTE));
}

inline void graphics_frame_buffer_draw_pixel(graphics_frame_buffer_t *frame_buffer,
                                      WORD x, WORD y, graphics_color_e color) {
  if (x < frame_buffer->width && y < frame_buffer->height) {
    frame_buffer->buffer[(y * frame_buffer->width) + x] = (BYTE)color;
  }
}

void graphics_frame_buffer_draw_line(graphics_frame_buffer_t *frame_buffer,
                                     WORD x1, WORD y1, WORD x2, WORD y2,
                                     graphics_color_e color) {
  if (!_ensure_bounds(x1, y1, x2, y2, frame_buffer->width,
                      frame_buffer->height)) {
    return;  // Out of bounds
  }

  const int16_t distance_x = abs(x2 - x1);
  const int16_t distance_y = abs(y2 - y1);
  const int16_t slope = _math_calculate_slope_distances(distance_x, distance_y);

  int16_t start = x1;
  int16_t end = x2;
  for (WORD idx = start; idx < end; idx++) {
    const int16_t f_idx = slope * (idx - start) + y1;
    if (f_idx < 0 || f_idx >= frame_buffer->height) {
      break;
    }

    graphics_frame_buffer_draw_pixel(frame_buffer, idx, f_idx, color);
  }
}

void graphics_frame_buffer_draw_rectangle(graphics_frame_buffer_t *frame_buffer,
                                          WORD x, WORD y, WORD width,
                                          WORD height, graphics_color_e color) {
  graphics_frame_buffer_draw_line(frame_buffer, x, y, x + width, y, color);
  graphics_frame_buffer_draw_line(frame_buffer, x, y, x, y + height, color);
  graphics_frame_buffer_draw_line(frame_buffer, x + width, y, x + width, y + height, color);
  graphics_frame_buffer_draw_line(frame_buffer, x, y + height, x + width, y + height, color);
}

void graphics_frame_buffer_fill_rectangle(graphics_frame_buffer_t *frame_buffer,
                                          WORD x, WORD y, WORD width,
                                          WORD height, graphics_color_e color) {
  for (WORD i = 0; i < width; i++) {
    for (WORD j = 0; j < height; j++) {
      graphics_frame_buffer_draw_pixel(frame_buffer, x + i, y + j, color);
    }
  }
}

void dump_graphics_frame_buffer(const graphics_frame_buffer_t *frame_buffer) {
  printf("Dumping graphics frame buffer:\n");
  for (WORD y = 0; y < frame_buffer->height; y++) {
    for (WORD x = 0; x < frame_buffer->width; x++) {
      printf("%01X", frame_buffer->buffer[(y * frame_buffer->width) + x]);
    }
    printf("\n");
    sleep_ms(10);  // Sleep to avoid flooding the console
  }
  printf("\n");
}
