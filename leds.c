/**
 * @file leds.c
 * @brief Implementation of functions for controlling LEDs.
 */

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "leds.h"

/**
 * @brief Activates the LED line to indicate a win using Morse code.
 *
 * This function displays the word "WIN" in Morse code on the LED line.
 * It uses a predefined Morse code pattern to blink the LEDs.
 */
void led_line_win() {
  unsigned char *mem_base;
  uint32_t val_line = 0xFFFFFFFF;
  int i;
  // Map the physical address of the SPI LED controller
mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)
    exit(1);
  //WIN in morse code
  int morse[13] = {1, 1, 3, 1, 3, 3, 1, 1, 1, 3, 3, 1, 1};
  int isLetter[13] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  // Set two RGB LEDs' color to green
  set_rgb_led_color(mem_base, 0x00FF00);
  // Blink the LEDs according to the Morse code pattern
  for (i=0; i<13; i++) {
 struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    if(isLetter[i] == 1){
      *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    }else{
      *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
    }
     loop_delay.tv_nsec *= morse[i];
     clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }
  // Turn off the LEDs
  set_rgb_led_color(mem_base, 0);
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
}

/**
 * @brief Activates the LED line to indicate a loss using Morse code.
 *
 * This function displays the word "LOST" in Morse code on the LED line.
 * It uses a predefined Morse code pattern to blink the LEDs.
 */
void led_line_lost() {
  unsigned char *mem_base;
  uint32_t val_line = 0xFFFFFFFF;
  int i;
  // Map the physical address of the SPI LED controller
mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)
    exit(1);
    // Morse code pattern for "LOST"
  int morse[21] = {1, 1, 4, 1, 1, 1, 1, 3, 4, 1, 4, 1, 4, 3, 1, 1, 1, 1, 1, 3, 4};
  int isLetter[21] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  set_rgb_led_color(mem_base, 0xFF0000);
  // Blink the LEDs according to the Morse code pattern
  for (i=0; i<21; i++) {
 struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    if(isLetter[i] == 1){
      *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    }else{
      *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
    }
     loop_delay.tv_nsec *= morse[i];
     clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
  }
  // Turn off the LEDs
  set_rgb_led_color(mem_base, 0);
  *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
}

/**
 * @brief Sets the color of an RGB LED.
 *
 * This function sets the color of both RGB LEDs to the same specified color.
 * It writes the color value to the appropriate memory addresses for the LEDs.
 */
void set_rgb_led_color(unsigned char* mem_base, unsigned int color){
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = color;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = color;
}

/**
 * @brief Sets the colors of two RGB LEDs.
 *
 * This function sets the colors of the two RGB LEDs to different specified colors.
 * It writes the first color value to the memory address for the first LED and
 * the second color value to the memory address for the second LED.
 */
void set_rgb_led_color2(unsigned char* mem_base, unsigned int color, unsigned int color2){
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = color;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = color2;
}
