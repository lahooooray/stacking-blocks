/**
 * @file stacking_blocks2.c
 * @brief Source file for the two-player Stacking Blocks game.
 *
 * This file contains the implementation of the functions for drawing a pixel and 
 * for the two-player stacking blocks game.
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
#include "stacking_blocks2.h"

#define BLOCK_HEIGHT 40         // Height of each block in pixels
#define BLOCK_WIDTH 75          // Width of each block in pixels
#define DISPLAY_WIDTH 480       // Width of the display in pixels
#define DISPLAY_HEIGHT 320      // Height of the display in pixels
#define MAX_BLOCK_NUMBER 6      // Maximum number of blocks
#define PLAYER1_LOST 0          // Indicates player 1 lost the game
#define PLAYER2_LOST 1          // Indicates player 2 lost the game
#define BOTH_LOST -1            // Indicates both players lost the game
#define BOTH_WON 2              // Indicates both players won the game
#define STILL_PLAYING 3         // Indicates the game is still in progress

unsigned short *fb_blocks2;

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
void draw_block_pixel(int x, int y, unsigned short color) {
  if (x>=0 && x<DISPLAY_WIDTH && y>=0 && y<DISPLAY_HEIGHT) {
    fb_blocks2[x+480*y] = color;
  }
}
 
/**
 * @brief Runs the two-player stacking blocks game.
 *
 * This function initializes the display and runs the main game loop for the two-player
 * stacking blocks game. The game involves stacking blocks and determines if the players win,
 * lose, or if the game is still ongoing. It updates the display in real-time and checks
 * for player input to adjust the stacking position.
 *
 * @param background_color The background color of the game display.
 * @param blocks_color1 The color of the blocks for the first player.
 * @param blocks_color2 The color of the blocks for the second player.
 * @return int Returns 0 if player 1 loses, 1 if player 2 loses, -1 if both lose, 2 if both win.
 */
int stacking_blocks_2players(unsigned short background_color, unsigned short blocks_color1, unsigned short blocks_color2){
  int result = STILL_PLAYING; // Result of the game, initially set to ongoing
  int player1_lost = STILL_PLAYING, player2_lost = STILL_PLAYING; 
  unsigned char *parlcd_mem_base, *mem_base;
  int i,j;
  int ptr;
  unsigned int c;
  fb_blocks2  = (unsigned short *)malloc(DISPLAY_HEIGHT*DISPLAY_WIDTH*2);
  // Map physical address for the display controller
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  if (parlcd_mem_base == NULL)
    exit(1);
  // Map physical address for the SPI LED controller
  mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  if (mem_base == NULL)
    exit(1);
 
  parlcd_hx8357_init(parlcd_mem_base);
  // Clear the display
  parlcd_write_cmd(parlcd_mem_base, 0x2c);
  ptr=0;
  for (i = 0; i < DISPLAY_HEIGHT ; i++) {
    for (j = 0; j < DISPLAY_WIDTH ; j++) {
      c = 0;
      fb_blocks2[ptr]=c;
      parlcd_write_data(parlcd_mem_base, fb_blocks2[ptr++]);
    }
  }
 
 int counter = 0; // Number of blocks stacked
 int width_range[MAX_BLOCK_NUMBER] = {0, 0, 0, 0, 0, 0}; // Range of block widths for player 1
 int width_range2[MAX_BLOCK_NUMBER] = {0, 0, 0, 0, 0, 0}; // Range of block widths for player 2
 int edges[6] = {440, 400, 360, 320, 280, 240}; // Edge positions of blocks for player 1
 int edges2[7] = {0, 40, 80, 120, 160, 200, 240}; // Edge positions of blocks for player 2
 int edge2 = 0, edge = 480; // Current edge positions for players 2 and 1 respectively
 // Main game loop
 while(counter < MAX_BLOCK_NUMBER){
    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 150 * 1000 * 1000;
    int speed1=0, speed2 =0, yy2 = 0, yy = 0;
    int range2 = 480, range = 0;
    int rand1 = rand()%(DISPLAY_HEIGHT-2*BLOCK_WIDTH+1)+BLOCK_WIDTH; // Random starting position for player 1
    int rand2 = rand()%(DISPLAY_HEIGHT-2*BLOCK_WIDTH+1)+BLOCK_WIDTH; // Random starting position for player 2
    // Block moving loop
    while (range2 >= edge2 && range < edge) {
      int r = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
      if (1&(r>>24)) {
        speed1+=7; // Increase speed if the knob is pressed for player 1
      }
      if(1&(r>>26)){
        speed2 += 7; // Increase speed if the knob is pressed for player 2
      }
      yy = ((r&0xff)*DISPLAY_HEIGHT)/256;
      yy2 = (((r>>16)&0xff)*DISPLAY_HEIGHT)/256;

      for (ptr = 0; ptr < DISPLAY_HEIGHT*DISPLAY_WIDTH ; ptr++) {
        fb_blocks2[ptr]=background_color;
      }
      for(int i=0; i<DISPLAY_HEIGHT; i++){
        for(int j=239; j<242; j++){
          draw_block_pixel(j, i, 0xffffff); ///< Draw the center line
        }
      }
      for(int n=0; n<counter; n++){
        for (i = 0; i < DISPLAY_HEIGHT ; i++) {
          for (j = 0; j < DISPLAY_WIDTH ; j++) {
            if(i >= width_range[n]-BLOCK_WIDTH && i < width_range[n] && j >= edges[n] && j <= edges[n]+BLOCK_HEIGHT){
              draw_block_pixel(j, i, blocks_color1); // Draw stacked blocks for player 1
            }
            if(i>= width_range2[n]-BLOCK_WIDTH && i < width_range2[n] && j >= edges2[n] && j <= edges2[n]+BLOCK_HEIGHT){
                draw_block_pixel(j, i, blocks_color2); // Draw stacked blocks for player 2
            }
          }
        }
      }
      // Draw the moving blocks
      for (j=0; j<BLOCK_WIDTH; j++) {
        int new_y = j-yy+rand1;
        if(new_y < 0){
          new_y+=DISPLAY_HEIGHT;
        }else if(new_y > DISPLAY_HEIGHT){
          new_y-=DISPLAY_HEIGHT;
        }
        int new_y2 = j-yy2+rand2;
        if(new_y2 > DISPLAY_HEIGHT){
          new_y2-=DISPLAY_HEIGHT;
        }else if(new_y2 < 0){
          new_y2+=DISPLAY_HEIGHT;
        }
        for (i=0; i<BLOCK_HEIGHT; i++) {
          range = DISPLAY_WIDTH/2+i+speed1;
          range2 = DISPLAY_WIDTH/2 - (i+speed2);
          if(range <= edge && range2 >= edge2){
            draw_block_pixel(range,new_y,blocks_color1);
            draw_block_pixel(range2,new_y2,blocks_color2);
          }else{
            width_range[counter] = new_y;
            width_range2[counter] = new_y2;
            break;
          }
        }
      }

      speed1+=15;
      speed2+=15;
  
      parlcd_write_cmd(parlcd_mem_base, 0x2c);
      for (ptr = 0; ptr < DISPLAY_WIDTH*DISPLAY_HEIGHT ; ptr++) {
          parlcd_write_data(parlcd_mem_base, fb_blocks2[ptr]);
      }
  
      clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    // Check for block alignment and update the game state
    if(counter>0){
      if(width_range2[counter] < width_range2[counter-1] - BLOCK_WIDTH/2 || width_range2[counter] > width_range2[counter-1] + BLOCK_WIDTH/2){
        player2_lost = PLAYER2_LOST;
      }
      if(width_range[counter] < width_range[counter-1] - BLOCK_WIDTH/2 || width_range[counter] > width_range[counter-1] + BLOCK_WIDTH/2){
        player1_lost = PLAYER1_LOST;
      }
      if(player1_lost == PLAYER1_LOST && player2_lost == PLAYER2_LOST){
        result = BOTH_LOST;
        break;
      }else if(player1_lost == PLAYER1_LOST && player2_lost == STILL_PLAYING){
        result = PLAYER1_LOST;
        break;
      }else if(player2_lost == PLAYER2_LOST && player1_lost == STILL_PLAYING){
        result = PLAYER2_LOST;
        break;
      }else if(counter == MAX_BLOCK_NUMBER-1 && player1_lost == player2_lost){
        result = BOTH_WON;
        break;
      }
    } 
    counter++;
    edge-=BLOCK_HEIGHT;
    edge2+=BLOCK_HEIGHT;
  }
  return result;
}
