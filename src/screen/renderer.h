/**
 * @file renderer.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Library for rendering a graphics frame buffer to a display.
 */
#pragma once

#include "frame.h"

#define CALC_INTERNAL_WIDTH ((SCREEN_WIDTH % 8 == 0) ? (SCREEN_WIDTH / 8) : (SCREEN_WIDTH / 8 + 1))
#define CALC_INTERNAL_HEIGH (SCREEN_HEIGHT)

#define SCREEN_HEIGHT 300
#define SCREEN_WIDTH 400

/**
 * @brief Attaches a graphics frame buffer to the renderer.
 *
 * This function sets the specified frame buffer as the target for rendering operations.
 *
 * @param frame_buffer A pointer to the `graphics_frame_buffer_t` structure to attach.
 */
void graphics_renderer_attach(graphics_frame_buffer_t *frame_buffer);

/**
 * @brief Detaches the currently attached graphics frame buffer from the renderer.
 *
 * This function removes any frame buffer currently set for rendering operations.
 */
void graphics_renderer_detach(void);

/**
 * @brief Updates the display with the contents of the attached frame buffer.
 *
 * This function renders the current frame buffer to the display.
 */
void graphics_renderer_update(void);
