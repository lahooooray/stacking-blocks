/**
 * @file drawing_words.c
 * @brief Implementation of functions for drawing text.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <termios.h> 

#include "drawing_words.h"
#include "font_types.h"

int scale = 4;

/**
 * @brief Draws a single pixel.
 *
 * This function sets the color of a pixel at the specified (x, y) position
 * in the framebuffer if the coordinates are within the display bounds.
 */
void draw_pixel(unsigned short *fb, font_descriptor_t *fdes, int x, int y, unsigned short color) {
    if (x >= 0 && x < 480 && y >= 0 && y < 320) {
        fb[x + 480 * y] = color;
    }
}

/**
 * @brief Draws a larger pixel (block of pixels).
 *
 * This function draws a block of pixels starting at (x, y) to create a larger pixel effect.
 */
void draw_pixel_big(unsigned short *fb, font_descriptor_t *fdes, int x, int y, unsigned short color) {
    int i, j;
    for (i = 0; i < scale; i++) {
        for (j = 0; j < scale; j++) {
            draw_pixel(fb, fdes, x + i, y + j, color);
        }
    }
}

/**
 * @brief Gets the width of a character in the specified font.
 *
 * Returns the width of the specified character in the font descriptor. If the
 * font does not have individual character widths, it returns the maximum width.
 */
int char_width(unsigned short *fb, font_descriptor_t *fdes, int ch) {
    int width;
    if (!fdes->width) {
        width = fdes->maxwidth;
    } else {
        width = fdes->width[ch - fdes->firstchar];
    }
    return width;
}

/**
 * @brief Draws a single character on the framebuffer.
 *
 * Draws a character at the specified (x, y) position using the provided font descriptor and color.
 * The character bitmap is scaled up by the global scale factor.
 */
void draw_char(unsigned short *fb, font_descriptor_t *fdes, int x, int y, char ch, unsigned short color) {
    int w = char_width(fb, fdes, ch);
    const font_bits_t *ptr;
    if ((ch >= fdes->firstchar) && (ch - fdes->firstchar < fdes->size)) {
        if (fdes->offset) {
            ptr = &fdes->bits[fdes->offset[ch - fdes->firstchar]];
        } else {
            int bw = (fdes->maxwidth + 15) / 16;
            ptr = &fdes->bits[(ch - fdes->firstchar) * bw * fdes->height];
        }
        int i, j;
        for (i = 0; i < fdes->height; i++) {
            font_bits_t val = *ptr;
            for (j = 0; j < w; j++) {
                if ((val & 0x8000) != 0) {
                    draw_pixel_big(fb, fdes, x + scale * j, y + scale * i, color);
                }
                val <<= 1;
            }
            ptr++;
        }
    }
}

/**
 * @brief Draws a string of characters on the framebuffer.
 *
 * Draws a string at the specified (x, y) position using the provided font descriptor and color.
 * Each character in the string is drawn in sequence, with the x-coordinate being incremented
 * by the character width times the scale factor.
 */
void draw_string(unsigned short *fb, font_descriptor_t *fdes, int x, int y, const char *str, unsigned short color) {
    while (*str) {
         draw_char(fb, fdes, x, y, *str, color);
        x += char_width(fb, fdes, *str) * scale;
        str++;
    }
}
