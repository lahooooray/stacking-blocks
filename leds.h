/**
 * @file leds.h
 * @brief Functions for controlling LEDs.
 */
#ifndef LEDS_H
#define LEDS_H

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Activates the LED line to indicate a win.
 *
 * This function configures the LED line to display a pattern or color
 * indicating a win condition.
 */
void led_line_win();

/**
 * @brief Activates the LED line to indicate a loss.
 *
 * This function configures the LED line to display a pattern or color
 * indicating a loss condition.
 */
void led_line_lost();

/**
 * @brief Sets the color of two RGB LEDs.
 *
 * This function sets the color of two RGB LEDs by writing the specified color value
 * to the appropriate memory address (the same color for both RGB LEDs)
 *
 * @param mem_base The base address of the memory-mapped LED control register.
 * @param color The color value to set the RGB LEDs to.
 */
void set_rgb_led_color(unsigned char* mem_base, unsigned int color);

/**
 * @brief Sets the colors of two RGB LEDs.
 *
 * This function sets the colors of two RGB LEDs by writing the specified color values
 * to the appropriate memory addresses.
 *
 * @param mem_base The base address of the memory-mapped LED control register.
 * @param color The color value to set the first RGB LED to.
 * @param color2 The color value to set the second RGB LED to.
 */
void set_rgb_led_color2(unsigned char* mem_base, unsigned int color, unsigned int color2);
#endif
