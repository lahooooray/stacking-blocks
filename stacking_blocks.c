/**
 * @file stacking_blocks.c
 * @brief Source file for the Stacking Blocks game.
 *
 * This file contains the implementation of the functions for drawing a pixel and 
 * for the single-player stacking block game.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <termios.h> 
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "stacking_blocks.h"

#define BLOCK_HEIGHT 60      // Height of each block in pixels
#define BLOCK_WIDTH 100      // Width of each block in pixels
#define DISPLAY_WIDTH 480    // Width of the display in pixels
#define DISPLAY_HEIGHT 320   // Height of the display in pixels
#define MAX_BLOCK_NUMBER 6   // Maximum number of blocks
#define PLAYER_LOST 0        // Indicates the player lost the game
#define PLAYER_WON 1         // Indicates the player won the game
unsigned short *fb_blocks;

/**
 * @brief Draws a single pixel for a block on the display.
 *
 * This function draws a single pixel at the specified coordinates with the given color,
 * but only if the coordinates are within the bounds of the display.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color of the pixel.
 */
void draw_block_pixel1(int x, int y, unsigned short color) {
  if (x>=0 && x<DISPLAY_WIDTH && y>=0 && y<DISPLAY_HEIGHT) {
    fb_blocks[x+480*y] = color;
  }
}
 
/**
 * @brief Runs the single-player stacking blocks game.
 *
 * This function initializes the display and runs the main game loop for the single-player
 * stacking blocks game. The game involves stacking blocks and determines if the player wins
 * or loses based on their performance. It updates the display in real-time and checks
 * for player input to adjust the stacking position.
 *
 * @param background_color The background color of the game display.
 * @param block_color The color of the blocks.
 * @return int Returns 1 if the player wins, 0 if the player loses.
 */
int stacking_block_1player(unsigned short background_color, unsigned short block_color) {
  int result = PLAYER_WON; // Result of the game, initially set to winning
  unsigned char *parlcd_mem_base, *mem_base;
  int i,j;
  int ptr;
  unsigned int c;
  fb_blocks  = (unsigned short *)malloc(320*480*2);
  // Map physical address for the display controller
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  if (parlcd_mem_base == NULL)
    exit(1);
  // Map physical address for the SPI LED controller
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)
    exit(1);
  // Initialize the display controller
  parlcd_hx8357_init(parlcd_mem_base);
  // Clear the display
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  ptr=0;
  for (i = 0; i < DISPLAY_HEIGHT ; i++) {
    for (j = 0; j < DISPLAY_WIDTH ; j++) {
      c = 0;
      fb_blocks[ptr]=c;
      parlcd_write_data(parlcd_mem_base, fb_blocks[ptr++]);
    }
  }
 
 int counter = 0; // Number of blocks stacked
 int width_range[MAX_BLOCK_NUMBER] = {0, 0, 0, 0, 0, 0}; // Range of block widths
 int edges[8] = {420, 360, 300, 240, 180, 120, 60, 0}; // Edge positions of blocks
 int edge = DISPLAY_WIDTH; // Current edge position
 // Main game loop
 while(counter < 8){
    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 150 * 1000 * 1000;
    int speed=0, yy=0;
    int range = 0;
    int random_position = rand()%(DISPLAY_HEIGHT-2*BLOCK_WIDTH+1)+BLOCK_WIDTH; // Random starting position
    // Block moving loop
    while (range < edge) {
      int r = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
      if (1&(r>>25)) {
        speed+=7; // Increase falling speed if the knob is pressed
      }
      yy = (((r >> 8)&0xff)*DISPLAY_HEIGHT)/256;
      for (ptr = 0; ptr < DISPLAY_HEIGHT*DISPLAY_WIDTH ; ptr++) {
        fb_blocks[ptr]=background_color;
      }
      // Draw stacked blocks
      for(int n=0; n<counter; n++){
        for (i = 0; i < DISPLAY_HEIGHT ; i++) {
          for (j = 0; j < DISPLAY_WIDTH ; j++) {
            if(i >= width_range[n]-BLOCK_WIDTH && i < width_range[n] && j >= edges[n] && j <= edges[n]+BLOCK_HEIGHT){
              draw_block_pixel1(j, i, block_color);
            }
          }
        }
      }
      // Draw the moving block
      for (j=0; j<BLOCK_WIDTH; j++) {
        int new_y = j-yy+random_position;
        if(new_y < 0){
          new_y+=DISPLAY_HEIGHT;
        }else if(new_y > DISPLAY_HEIGHT){
          new_y -= DISPLAY_HEIGHT;
        }
        for (i=0; i<BLOCK_HEIGHT; i++) {
          range = i+speed;
          if(range <= edge){
            draw_block_pixel1(range, new_y, block_color);
          }else{
            width_range[counter] = new_y;
            break;
          }
        }
      }
      speed+=15;
      // Update the display
      parlcd_write_cmd(parlcd_mem_base, 0x2c);
      for (ptr = 0; ptr < 480*320 ; ptr++) {
          parlcd_write_data(parlcd_mem_base, fb_blocks[ptr]);
      }
      clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    // Check if the player loses
    if(counter>0 && (width_range[counter] < width_range[counter-1] - BLOCK_WIDTH/2 || width_range[counter] > width_range[counter-1] + BLOCK_WIDTH/2)){
      result = PLAYER_LOST;
      break;
    }
    counter++;
    edge-=BLOCK_HEIGHT;
  }
  return result;
}

