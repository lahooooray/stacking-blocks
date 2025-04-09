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
#include "font_types.h"
#include "drawing_words.h"
#include "leds.h"
#include "stacking_blocks.h"
#include "stacking_blocks2.h"

#define DARK_THEME_BACK 0
#define DARK_THEME_FONT 0x7BEF

#define PINK_THEME_BACK 0xED3A
#define PINK_THEME_FONT 0xDB15

#define FONT_SELECTED 0xFFFF

unsigned short *fb;
font_descriptor_t *fdes;

/**
 * @enum app_state_t
 * @brief Enumeration of application states.
 */
typedef enum {
    STATE_MENU,
    STATE_GAME_1,
    STATE_GAME_2,
    STATE_THEME
} app_state_t;

/**
 * @brief Clears the screen with the given color.
 *
 * @param color The color to fill the screen with.
 */
void clear_screen(unsigned int color);

/**
 * @brief Updates the display with the current framebuffer content.
 *
 * @param parlcd_mem_base The base address of the parallel LCD memory.
 */
void update_display(unsigned char *parlcd_mem_base);

/**
 * @brief Displays the menu on the screen.
 *
 * @param parlcd_mem_base The base address of the parallel LCD memory.
 * @param selected_phrase The index of the selected menu item.
 * @param back The background color.
 * @param font_color The font color.
 */
void display_menu(unsigned char *parlcd_mem_base, int selected_phrase, unsigned int back, unsigned int font_color);

/**
 * @brief Displays a screen with a single phrase.
 *
 * @param parlcd_mem_base The base address of the parallel LCD memory.
 * @param back The background color.
 * @param phrase The phrase to display.
 */
void display_screen(unsigned char *parlcd_mem_base,unsigned int back, char* phrase);

/**
 * @brief Main function of the program. Checks the status of the game, 
 * which option on the menu is chosen, and changes to screen according to the 
 * result of the game. 
 */
int main(int argc, char *argv[]) {
    unsigned char *parlcd_mem_base, *mem_base;
    int ptr;
    fb = (unsigned short *)malloc(320 * 480 * 2);
    unsigned int curr_back = DARK_THEME_BACK;
    unsigned int curr_font = DARK_THEME_FONT;
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);

    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (mem_base == NULL)
        exit(1);

    parlcd_hx8357_init(parlcd_mem_base);
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int i = 0; i < 320; i++) {
        for (int j = 0; j < 480; j++) {
            fb[i * 480 + j] = 0;
            parlcd_write_data(parlcd_mem_base, fb[i * 480 + j]);
        }
    }
    struct timespec loop_delay;
    loop_delay.tv_sec = 0;
    loop_delay.tv_nsec = 150 * 1000 * 1000;
    fdes = &font_winFreeSystem14x16;
    int selected_phrase = 0;
    app_state_t state = STATE_MENU;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
    // game loop
    while (1) {
        int r = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        int knob_value = (r >> 8) & 0xFF;
        static int last_knob_value = 0;
        // selecting a different option on the MENU screen as 
        // the green knob is rotated.
        if (knob_value != last_knob_value) {
            if (knob_value > last_knob_value) {
                selected_phrase = (selected_phrase + 1) % 3;
            } else {
                selected_phrase = (selected_phrase - 1 + 3) % 3;
            }
            last_knob_value = knob_value;
        }
        // processing press on the green knob and changing state of the 
        // application accordingly
        if ((r>>25) & 1) {
            if (state == STATE_MENU) {
                if (selected_phrase == 0) {
                    state = STATE_GAME_1;
                }else if(selected_phrase == 1){
                    state = STATE_GAME_2;
                } else if (selected_phrase == 2) {
                    state = STATE_THEME;
                }
            } else {
                state = STATE_MENU;
            }
        }
        if (state == STATE_MENU) {
            display_menu(parlcd_mem_base, selected_phrase, curr_back, curr_font);
        } else if (state == STATE_GAME_1) {
            int game_result = stacking_block_1player(curr_back, curr_font);
            if(game_result==1){
                display_screen(parlcd_mem_base,curr_back, "YOU WON!");
                set_rgb_led_color(mem_base, 0x00FF00);
                led_line_win();
            }else{
                display_screen(parlcd_mem_base, curr_back, "YOU LOST!");
                set_rgb_led_color(mem_base, 0xFF0000);
                led_line_lost();
                struct timespec screen_delay = {.tv_sec = 0, .tv_nsec = 300 * 1000 * 1000};
                clock_nanosleep(CLOCK_MONOTONIC, 0, &screen_delay, NULL);
            }
            sleep(3);
            set_rgb_led_color(mem_base, 0);
        }else if(state == STATE_GAME_2){
            int game_result = stacking_blocks_2players(curr_back, curr_font, FONT_SELECTED);
            if(game_result==1){
                if(curr_font == DARK_THEME_FONT){
                    display_screen(parlcd_mem_base, curr_back, "GREY WON!");
                }else{
                    display_screen(parlcd_mem_base, curr_back, "PINK WON!");
                }
                set_rgb_led_color2(mem_base, 0xFF0000, 0x00FF00);
            }else if (game_result == 0){
                display_screen(parlcd_mem_base, curr_back, "WHITE WON!");
                set_rgb_led_color2(mem_base, 0x00FF00, 0xFF0000);
            }else{
                display_screen(parlcd_mem_base, curr_back, "DRAW!");
                set_rgb_led_color(mem_base, 0x0000FF);
                struct timespec screen_delay = {.tv_sec = 0, .tv_nsec = 300 * 1000 * 1000};
                clock_nanosleep(CLOCK_MONOTONIC, 0, &screen_delay, NULL);
            }
            sleep(3);
            set_rgb_led_color(mem_base, 0);
        }else if(state == STATE_THEME){
            if(curr_back == DARK_THEME_BACK){
                curr_back = PINK_THEME_BACK;
                curr_font = PINK_THEME_FONT;
            }else{
                curr_back = DARK_THEME_BACK;
                curr_font = DARK_THEME_FONT;
            }
        }
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
        state = STATE_MENU;
    }
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480 * 320; ptr++) {
        parlcd_write_data(parlcd_mem_base, 0);
    }
    return 0;
}

void clear_screen(unsigned int color) {
    for (int ptr = 0; ptr < 320 * 480; ptr++) {
        fb[ptr] = color;
    }
}

void update_display(unsigned char *parlcd_mem_base) {
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (int ptr = 0; ptr < 480 * 320; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

void display_menu(unsigned char *parlcd_mem_base, int selected_phrase, unsigned int back, unsigned int font_color) {
    char *phrases[] = {"1 player", "2 players", "change theme"};
    int num_phrases = 3;

    int x = 60;
    int y_start = 50;
    int y_gap = 50;


    clear_screen(back);
    unsigned int color = font_color;
    draw_string(fb, fdes, x, y_start, "MENU", color);
    y_start = 100;
    for (int k = 0; k < num_phrases; k++) {
        int y = y_start + k * y_gap;
        unsigned int color = (k == selected_phrase) ? FONT_SELECTED : font_color;
        draw_string(fb, fdes, x, y, phrases[k], color);
    }
    update_display(parlcd_mem_base);
}

void display_screen(unsigned char *parlcd_mem_base, unsigned int back, char* phrase) {
    clear_screen(back);
    draw_string(fb, fdes, 80, 120, phrase, FONT_SELECTED);
    update_display(parlcd_mem_base);
}
