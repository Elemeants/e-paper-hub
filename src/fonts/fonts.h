/**
 * @file fonts.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Font definitions for the graphics library
 *
 * This header file defines the font structure and provides access to font data used in the graphics library.
 * It includes the definition of a font table structure and a function to retrieve font data for specific characters.
 */
#pragma once

/**
 * @brief Font character definition
 *
 * This structure defines the properties of a single font character, including its width, height, and bitmap data.
 */
typedef struct {
  /**
   * @brief Width of the character in pixels
   */
  const uint8_t c_width;

  /**
   * @brief Height of the character in pixels
   */
  const uint8_t c_height;

  /**
   * @brief Bitmap data for the character
   */
  const uint8_t* table;
} font_table_t;

/**
 * @brief Font data for 16-point font
 *
 * This variable holds the font data for a 16-point font, including character dimensions and bitmap data.
 */
extern const font_table_t font_16;

/**
 * @brief Retrieves the bitmap data for a specific character in the 16-point font.
 *
 * This function calculates the offset in the font table for the given character and returns a pointer to its bitmap data.
 *
 * @param c The character to retrieve the font data for.
 * @return A pointer to the bitmap data for the specified character.
 */
const uint8_t* font16_get_entry(char c);
