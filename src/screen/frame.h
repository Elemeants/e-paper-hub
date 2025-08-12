/**
 * @file frame.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Library for managing a graphics frame buffer.
 *
 * This library provides functions to create, destroy, clear, and draw on a graphics frame buffer.
 * It supports drawing pixels, lines, rectangles, and filling shapes with colors.
 *
 * A `graphics_frame_buffer_t` is the key structure that holds the frame buffer data,
 * including its width, height, and a pointer to the pixel data buffer.
 *
 * The `graphics_frame_buffer_t#buffer` represents the pixel data in the structure of `ROW_1`,
 * `ROW_2`, ..., `ROW_N`, where each row contains `width` pixels.
 */
#pragma once

#include <utils/defs.h>

/**
 * @brief Structure representing a graphics frame buffer.
 *
 * This structure holds the pixel data for a single frame, including its width and height.
 */
typedef struct {
  BYTE *buffer;
  WORD width;
  WORD height;
} graphics_frame_buffer_t;

/**
 * @brief Enumeration of colors supported in the graphics frame buffer.
 *
 * This enum defines a set of colors that can be used for drawing operations.
 */
typedef enum {
  GRAPHICS_COLOR_BLACK,
  GRAPHICS_COLOR_WHITE,
  GRAPHICS_COLOR_RED,
} graphics_color_e;

/**
 * @brief Creates a new graphics frame buffer with the specified width and height.
 *
 * This function allocates memory for a new frame buffer and initializes its dimensions.
 *
 * @param width The width of the frame buffer in pixels.
 * @param height The height of the frame buffer in pixels.
 * @return A `graphics_frame_buffer_t` structure representing the created frame buffer.
 */
graphics_frame_buffer_t graphics_frame_buffer_create(WORD width, WORD height);

/**
 * @brief Destroys a graphics frame buffer, freeing its allocated memory.
 *
 * This function releases the memory used by the frame buffer's pixel data.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to be destroyed.
 */
void graphics_frame_buffer_destroy(graphics_frame_buffer_t *frame_buffer);

/**
 * @brief Clears the frame buffer, filling it with the specified color.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to be cleared.
 * @param color The color to fill the frame buffer with.
 */
void graphics_frame_buffer_clear(graphics_frame_buffer_t *frame_buffer, graphics_color_e color);

/**
 * @brief Draws a pixel on the frame buffer.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to draw on.
 * @param x The x-coordinate of the pixel to draw.
 * @param y The y-coordinate of the pixel to draw.
 * @param color The color of the pixel to draw.
 */
void graphics_frame_buffer_draw_pixel(graphics_frame_buffer_t *frame_buffer, WORD x, WORD y, graphics_color_e color);

/**
 * @brief Draws a line on the frame buffer.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to draw on.
 * @param x1 The x-coordinate of the starting point of the line.
 * @param y1 The y-coordinate of the starting point of the line.
 * @param x2 The x-coordinate of the ending point of the line.
 * @param y2 The y-coordinate of the ending point of the line.
 * @param color The color of the line to draw.
 */
void graphics_frame_buffer_draw_line(graphics_frame_buffer_t *frame_buffer, WORD x1, WORD y1, WORD x2, WORD y2, graphics_color_e color);

/**
 * @brief Draws a rectangle on the frame buffer.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to draw on.
 * @param x The x-coordinate of the top-left corner of the rectangle.
 * @param y The y-coordinate of the top-left corner of the rectangle.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 * @param color The color of the rectangle to draw.
 */
void graphics_frame_buffer_draw_rectangle(graphics_frame_buffer_t *frame_buffer, WORD x, WORD y, WORD width, WORD height, graphics_color_e color);

/**
 * @brief Fills a rectangle on the frame buffer with the specified color.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to draw on.
 * @param x The x-coordinate of the top-left corner of the rectangle.
 * @param y The y-coordinate of the top-left corner of the rectangle.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 * @param color The color to fill the rectangle with.
 */
void graphics_frame_buffer_fill_rectangle(graphics_frame_buffer_t *frame_buffer, WORD x, WORD y, WORD width, WORD height, graphics_color_e color);

/**
 * @brief Dumps the contents of the frame buffer to the console.
 *
 * This function prints the pixel data of the frame buffer to the console for debugging purposes.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to dump.
 */
void dump_graphics_frame_buffer(const graphics_frame_buffer_t *frame_buffer);

void graphics_frame_buffer_draw_text(graphics_frame_buffer_t *frame_buffer, WORD x, WORD y, const char *text, graphics_color_e color);

void graphics_frame_buffer_draw_bitmap(graphics_frame_buffer_t *frame_buffer,
                                      WORD x, WORD y,
                                      const uint8_t *data, WORD width, WORD height,
                                      graphics_color_e color);

// TODO: Later implementation
void graphics_frame_buffer_draw_circle(graphics_frame_buffer_t *frame_buffer, WORD x, WORD y, WORD radius, graphics_color_e color);
void graphics_frame_buffer_fill_circle(graphics_frame_buffer_t *frame_buffer, WORD x, WORD y, WORD radius, graphics_color_e color);
