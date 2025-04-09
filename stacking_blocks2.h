/**
 * @file stacking_blocks2.h
 * @brief Header file for the Stacking Blocks game.
 *
 * This file contains the function declarations for drawing a pixel and for the 
 * two-player stacking blocks game.
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
void draw_block_pixel(int x, int y, unsigned short color);

/**
 * @brief Runs the two-player stacking blocks game.
 *
 * This function initializes the display and runs the main game loop for the two-player
 * stacking blocks game. The game involves stacking blocks and determines the outcome
 * based on the performance of both players.
 *
 * @param background_color The background color of the game display.
 * @param blocks_color1 The color of the blocks for the first player.
 * @param blocks_color2 The color of the blocks for the second player.
 * @return int Returns 1 if the first player wins, 2 if the second player wins, 0 if it is a draw.
 */
int stacking_blocks_2players(unsigned short background_color, unsigned short blocks_color1, unsigned short blocks_color2);
#endif
