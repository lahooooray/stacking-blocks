/**
 * @file stacking_blocks.h
 * @brief Header file for the Stacking Blocks game.
 *
 * This file contains the function declarations for drawing a pixel and for the 
 * single-player stacking block game.
 */

#ifndef STACKING_BLOCKS_H
#define STACKING_BLOCKS_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/**
 * @brief Draws a single pixel for a block on the display.
 *
 * This function draws a single pixel at the specified coordinates with the given color.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
 */
void draw_block_pixel1(int x, int y, unsigned short color);

/**
 * @brief Runs the single-player stacking blocks game.
 *
 * This function initializes the display and runs the main game loop for the single-player
 * stacking blocks game. The game involves stacking blocks and determines if the player wins
 * or loses based on their performance.
 *
 * @param background_color The background color of the game display.
 * @param block_color The color of the blocks.
 * @return int Returns 1 if the player wins, 0 if the player loses.
 */
int stacking_block_1player(unsigned short background_color, unsigned short block_color);
#endif
