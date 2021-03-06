/**
 * FlappyBird-N64 - ui.h
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_UI_H
#define __FLAPPY_UI_H

#include "system.h"
#include "graphics.h"

#include "bird.h"
#include "background.h"

/* UI definitions */

#define UI_DEATH_FLASH_MS   80
#define UI_FLASH_COLOR      graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )

#define UI_DEATH_HEADING_DELAY  600
#define UI_DEATH_BOARD_DELAY    1500
#define UI_DEATH_BOARD_DY_MS    200.0
#define UI_DEATH_SCORE_DELAY    48

#define UI_DARK_COLOR       graphics_make_color( 0x57, 0x37, 0x47, 0xFF )
#define UI_LIGHT_COLOR      graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )
#define UI_CLEAR_COLOR      graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

typedef enum ui_heading
{
    UI_HEADING_GET_READY,
    UI_HEADING_GAME_OVER
} ui_heading_t;

typedef enum ui_sprite
{
    UI_SPRITE_LOGO,
    UI_SPRITE_HEADINGS,
    UI_SPRITE_HOWTO,
    UI_SPRITE_SCOREBOARD,
    UI_SPRITE_MEDAL,
    UI_SPRITE_FONT_LARGE,
    UI_SPRITE_FONT_MED,
    UI_SPRITE_NEW,
    UI_SPRITE_SPARKLE,
    UI_NUM_SPRITES
} ui_sprite_t;

typedef enum ui_medal_score
{
    UI_MEDAL_SCORE_BRONZE   = 10,
    UI_MEDAL_SCORE_SILVER   = 20,
    UI_MEDAL_SCORE_GOLD     = 30,
    UI_MEDAL_SCORE_PLATINUM = 40
} ui_medal_score_t;

typedef enum ui_medal_stride
{
    UI_MEDAL_STRIDE_BRONZE,
    UI_MEDAL_STRIDE_SILVER,
    UI_MEDAL_STRIDE_GOLD,
    UI_MEDAL_STRIDE_PLATINUM
} ui_medal_stride_t;

typedef struct
{
    bird_state_t state;
    /* Scoring */
    u16 last_score;
    u16 high_score;
    bool new_high_score;
    /* Titles */
    bg_time_mode_t time_mode;
    u32 text_color;
    u32 shadow_color;
    u32 clear_color;
    sprite_t *sprites[UI_NUM_SPRITES];
    /* Death */
    bool did_flash;
    bool flash_draw;
    u32 flash_color;
    u32 hit_ms;
    /* Game Over */
    u32 dead_ms;
    bool did_gameover;
    bool heading_draw;
    bool board_draw;
    bool score_draw;
    bool medal_draw;
    /* Scoreboard animations */
    u32 board_ms;
    u16 board_y;
    u32 score_ms;
    u16 last_score_acc;
    u16 high_score_acc;
} ui_t;

/* UI functions */

ui_t ui_setup(const background_t *bg);

void ui_free(ui_t *ui);

void ui_tick(ui_t *ui, const bird_t *bird, const background_t *bg);

void ui_draw(const ui_t *ui);

#endif
