/**
 * @file drawing_words.h
 * @brief Functions for drawing text.
 */

#ifndef DRAWING_WORDS_H
#define DRAWING_WORDS_H

#include <stdint.h>
#include "font_types.h"

/**
 * @brief Draws a single pixel.
 *
 * @param fb The framebuffer where the pixel will be drawn.
 * @param fdes The font descriptor.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
 */
void draw_pixel(unsigned short *fb, font_descriptor_t *fdes, int x, int y, unsigned short color);

/**
 * @brief Draws a larger pixel on the framebuffer.
 *
 * @param fb The framebuffer where the pixel will be drawn.
 * @param fdes The font descriptor.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
 */
void draw_pixel_big(unsigned short *fb, font_descriptor_t *fdes, int x, int y, unsigned short color);

/**
 * @brief Gets the width of a character.
 *
 * @param fb The framebuffer.
 * @param fdes The font descriptor.
 * @param ch The character to get the width of.
 * @return The width of the character.
 */
int char_width(unsigned short *fb, font_descriptor_t *fdes, int ch);

/**
 * @brief Draws a single character.
 *
 * @param fb The framebuffer where the character will be drawn.
 * @param fdes The font descriptor.
 * @param x The x-coordinate where the character will start.
 * @param y The y-coordinate where the character will start.
 * @param ch The character to be drawn.
 * @param color The color of the character.
 */
void draw_char(unsigned short *fb, font_descriptor_t *fdes, int x, int y, char ch, unsigned short color);

/**
 * @brief Draws a string of characters.
 *
 * @param fb The framebuffer where the string will be drawn.
 * @param fdes The font descriptor.
 * @param x The x-coordinate where the string will start.
 * @param y The y-coordinate where the string will start.
 * @param str The string to be drawn.
 * @param color The color of the string.
 */
void draw_string(unsigned short *fb, font_descriptor_t *fdes, int x, int y, const char *str, unsigned short color); 
#endif
