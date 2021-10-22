/**
 * FlappyBird-N64 - ui.c
 *
 * Copyright 2021, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ui.h"

#include "system.h"
#include "gfx.h"
#include "sfx.h"
#include "background.h"
#include "bird.h"

/* UI definitions */

// This should be set by the project Makefile
#ifndef ROM_VERSION
#define ROM_VERSION         ""
#endif

#define UI_SCORE_MAX_DIGITS     ((size_t) 5)

#define UI_DEATH_FLASH_MS       ((int) 80)
#define UI_DEATH_HEADING_DELAY  ((int) 600)
#define UI_DEATH_BOARD_DELAY    ((int) 1500)
#define UI_DEATH_BOARD_DY_MS    ((float) 200.0)
#define UI_DEATH_SCORE_DELAY    ((int) 48)

#define UI_DARK_COLOR           graphics_make_color( 0x57, 0x37, 0x47, 0xFF )
#define UI_LIGHT_COLOR          graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )
#define UI_CLEAR_COLOR          graphics_make_color( 0x00, 0x00, 0x00, 0x00 )
#define UI_FLASH_COLOR          graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )

typedef enum
{
    UI_HEADING_GET_READY,
    UI_HEADING_GAME_OVER
} ui_heading_t;

typedef enum
{
    UI_MEDAL_SCORE_BRONZE   = 10,
    UI_MEDAL_SCORE_SILVER   = 20,
    UI_MEDAL_SCORE_GOLD     = 30,
    UI_MEDAL_SCORE_PLATINUM = 40
} ui_medal_score_t;

typedef enum
{
    UI_MEDAL_STRIDE_BRONZE,
    UI_MEDAL_STRIDE_SILVER,
    UI_MEDAL_STRIDE_GOLD,
    UI_MEDAL_STRIDE_PLATINUM
} ui_medal_stride_t;

typedef enum
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

static const char * const UI_SPRITE_FILES[UI_NUM_SPRITES] = {
    "gfx/logo.sprite",
    "gfx/headings.sprite",
    "gfx/how-to.sprite",
    "gfx/scoreboard.sprite",
    "gfx/medal.sprite",
    "gfx/font-large.sprite",
    "gfx/font-medium.sprite",
    "gfx/new.sprite",
    "gfx/sparkle.sprite"
};

typedef struct ui_s
{
    bird_state_t state;
    /* Scoring */
    int last_score;
    int high_score;
    bool new_high_score;
    /* Titles */
    bg_time_mode_t time_mode;
    gfx_color_t text_color;
    gfx_color_t shadow_color;
    gfx_color_t clear_color;
    sprite_t * sprites[UI_NUM_SPRITES];
    /* Death */
    bool did_flash;
    bool flash_draw;
    gfx_color_t flash_color;
    ticks_t hit_ms;
    /* Game Over */
    ticks_t dead_ms;
    bool did_gameover;
    bool heading_draw;
    bool board_draw;
    bool score_draw;
    bool medal_draw;
    /* Scoreboard animations */
    ticks_t board_ms;
    int board_y;
    ticks_t score_ms;
    int last_score_acc;
    int high_score_acc;
} ui_t;

/* UI implementation */

inline static void ui_set_time_mode(ui_t * ui, bg_time_mode_t time_mode)
{
    ui->time_mode = time_mode;
    if ( time_mode == BG_DAY_TIME )
    {
        ui->text_color = UI_DARK_COLOR;
        ui->shadow_color = UI_LIGHT_COLOR;
    }
    else
    {
        ui->text_color = UI_LIGHT_COLOR;
        ui->shadow_color = UI_DARK_COLOR;
    }
}

ui_t * ui_init(const background_t * bg)
{
    ui_t * ui = malloc( sizeof(ui_t) );
    if (ui == NULL) return NULL;
    memset( ui, 0, sizeof(ui_t) );
    ui->flash_color = UI_FLASH_COLOR;
    ui->board_y = gfx->height;
    ui_set_time_mode( ui, background_get_time_mode( bg ) );
    // Load the sprites
    for (size_t i = 0; i < UI_NUM_SPRITES; i++)
    {
        ui->sprites[i] = read_dfs_sprite( UI_SPRITE_FILES[i] );
    }
    return ui;
}

void ui_free(ui_t * ui)
{
    for (size_t i = 0; i < UI_NUM_SPRITES; i++)
    {
        free( ui->sprites[i] );
        ui->sprites[i] = NULL;
    }
    free( ui );
}

inline static void ui_bird_tick(ui_t * ui, const bird_t * bird)
{
    /* Synchronize bird state to UI */
    ui->state = bird->state;
    switch ( ui->state )
    {
        case BIRD_STATE_DEAD:
            ui->dead_ms = bird->dead_ms;
        case BIRD_STATE_DYING:
            ui->hit_ms = bird->hit_ms;
            break;
        default:
            break;
    }
    /* High scoring */
    ui->last_score = bird->score;
    if ( bird->score == 0 )
    {
        ui->new_high_score = false;
    }
    if ( bird->score > ui->high_score )
    {
        ui->high_score = bird->score;
        ui->new_high_score = true;
    }
}

inline static void ui_flash_tick(ui_t * ui)
{
    const ticks_t ticks_ms = get_total_ms();
    /* Flash the screen for a split second after the bird dies */
    if ( ui->state == BIRD_STATE_DYING ||
         ui->state == BIRD_STATE_DEAD )
    {
        if ( !ui->did_flash )
        {
            const bool was_flash_draw = ui->flash_draw;
            ui->flash_draw = ticks_ms - ui->hit_ms <= UI_DEATH_FLASH_MS;
            if ( was_flash_draw && !ui->flash_draw )
            {
                ui->did_flash = true;
            }
        }
    }
    else
    {
        ui->did_flash = false;
    }
}

inline static void ui_gameover_tick(ui_t * ui)
{
    if ( ui->state != BIRD_STATE_DEAD )
    {
        ui->did_gameover = false;
        return;
    }
    if ( ui->did_gameover )
    {
        // TODO Medal sparkles
        return;
    }
    /* Animate the Game Over UI */
    const ticks_t ticks_ms = get_total_ms();
    const int dead_diff_ms = ticks_ms - ui->dead_ms;
    /* Only show the scores and medal after the scoreboard appears */
    ui->score_draw = false;
    ui->medal_draw = false;
    /* Show the game over heading and play a sound */
    const bool was_heading_draw = ui->heading_draw;
    ui->heading_draw = dead_diff_ms >= UI_DEATH_HEADING_DELAY;
    if ( !was_heading_draw && ui->heading_draw )
    {
        sfx_play( SFX_SWOOSH );
    }
    /* Show the scoreboard and play a sound */
    const bool was_board_draw = ui->board_draw;
    ui->board_draw = dead_diff_ms >= UI_DEATH_BOARD_DELAY;
    if ( !was_board_draw && ui->board_draw )
    {
        sfx_play( SFX_SWOOSH );
        ui->board_ms = ticks_ms;
    }
    if ( ui->board_draw )
    {
        sprite_t * const scoreboard = ui->sprites[UI_SPRITE_SCOREBOARD];
        const int board_diff_ms = ticks_ms - ui->board_ms;
        const int max_y = gfx->height;
        const int center_y = max_y >> 1;
        const int min_y = center_y - (scoreboard->height >> 1);
        ui->score_draw = board_diff_ms >= UI_DEATH_BOARD_DY_MS;
        if ( !ui->score_draw )
        {
            /* Pop the scoreboard up from the bottom */
            const int y_diff = max_y - min_y;
            const float y_factor = board_diff_ms / UI_DEATH_BOARD_DY_MS;
            if ( y_factor >= 0.0 && y_factor < 1.0 )
            {
                ui->board_y = max_y - (y_diff * y_factor);
            }
            else
            {
                ui->board_y = max_y - y_diff;
            }
            /* Reset the score accumulator */
            ui->score_ms = ticks_ms;
            ui->last_score_acc = 0;
        }
        else
        {
            ui->board_y = min_y;
        }
    }
    if ( ui->score_draw )
    {
        if ( ui->last_score_acc < ui->last_score ||
             ui->high_score_acc < ui->high_score )
        {
            const int score_diff_ms = ticks_ms - ui->score_ms;
            if ( score_diff_ms >= UI_DEATH_SCORE_DELAY )
            {
                if ( ui->last_score_acc < ui->last_score )
                {
                    ui->last_score_acc++;
                }
                else if ( ui->high_score_acc < ui->high_score )
                {
                    ui->high_score_acc++;
                }
                ui->score_ms = ticks_ms;
            }
        }
        else
        {
            ui->medal_draw = true;
            ui->did_gameover = true;
        }
    }
}

void ui_tick(ui_t * ui, const bird_t * bird, const background_t * bg)
{
    /* Synchronize background state to UI */
    const bg_time_mode_t bg_time_mode = background_get_time_mode( bg );
    if ( ui->time_mode != bg_time_mode )
    {
        ui_set_time_mode( ui, bg_time_mode );
    }
    ui_bird_tick( ui, bird );
    ui_flash_tick( ui );
    ui_gameover_tick( ui );
}

/* Below this point there be magic numbers! */

inline static void ui_logo_draw(const ui_t * ui)
{
    sprite_t * const logo = ui->sprites[UI_SPRITE_LOGO];

    gfx_detach_rdp();
    const display_context_t disp = gfx->disp;

    const int center_x = (gfx->width >> 1);
    const int center_y = (gfx->height >> 1);
    const int logo_x = center_x - (logo->width >> 1);
    const int logo_y = center_y - (logo->height * 3.5);

    graphics_draw_sprite_trans( disp, logo_x, logo_y, logo );

    const char * const credit1_str = "Game by .GEARS";
    const int credit1_w = strlen(credit1_str) * 8;
    const int credit1_x = center_x - (credit1_w >> 1);
    const int credit1_y = gfx->height - 80;

    const char * const credit2_str = "N64 Port by Meeq";
    const int credit2_w = strlen(credit2_str) * 8;
    const int credit2_x = center_x - (credit2_w >> 1);
    const int credit2_y = gfx->height - 62;

    const char * const version_str = ROM_VERSION;
    const int version_w = strlen(version_str) * 8;
    const int version_x = gfx->width - 32 - version_w;
    const int version_y = gfx->height - 32;

    /* Draw a shadow under the text */
    graphics_set_color( ui->shadow_color, ui->clear_color );
    graphics_draw_text( disp, credit1_x+1, credit1_y+1, credit1_str );
    graphics_draw_text( disp, credit2_x+1, credit2_y+1, credit2_str );
    if (version_w)
        graphics_draw_text( disp, version_x+1, version_y+1, version_str );

    /* Draw the same text on top of the shadow */
    graphics_set_color( ui->text_color, ui->clear_color );
    graphics_draw_text( disp, credit1_x, credit1_y, credit1_str );
    graphics_draw_text( disp, credit2_x, credit2_y, credit2_str );
    if (version_w)
        graphics_draw_text( disp, version_x, version_y, version_str );
}

inline static void ui_heading_draw(const ui_t * ui, int stride)
{
    sprite_t * const headings = ui->sprites[UI_SPRITE_HEADINGS];

    gfx_detach_rdp();

    const int center_x = (gfx->width >> 1);
    const int center_y = (gfx->height >> 1);
    const int x = center_x - (headings->width >> 1);
    const int y = center_y - 70;

    graphics_draw_sprite_trans_stride( gfx->disp, x, y, headings, stride );
}

inline static void ui_howto_draw(const ui_t * ui)
{
    sprite_t * const howto = ui->sprites[UI_SPRITE_HOWTO];

    gfx_detach_rdp();

    const int center_x = (gfx->width >> 1);
    const int center_y = (gfx->height >> 1);
    const int x = center_x - (howto->width >> 1);
    const int y = center_y - (howto->height / 1.45);

    graphics_draw_sprite_trans( gfx->disp, x, y, howto );
}

inline static void ui_score_draw(const ui_t * ui)
{
    uint16_t score = ui->last_score;
    sprite_t * const font = ui->sprites[UI_SPRITE_FONT_LARGE];

    gfx_detach_rdp();
    const display_context_t disp = gfx->disp;

    size_t i = 0, num_digits;
    int digits[UI_SCORE_MAX_DIGITS];
    do
    {
        digits[i] = score % 10;
        score /= 10;
        num_digits = ++i;
    }
    while (score != 0 && num_digits < UI_SCORE_MAX_DIGITS);

    const int digit_w = font->width / font->hslices;
    const int score_w = digit_w * num_digits;
    const int center_x = gfx->width >> 1;
    const int y = 20;

    int x = center_x + (score_w >> 1) - digit_w;
    for (i = 0; i < num_digits; i++)
    {
        graphics_draw_sprite_trans_stride( disp, x, y, font, digits[i] );
        x -= digit_w;
    }
}

inline static void ui_scoreboard_draw(const ui_t * ui)
{
    sprite_t * const scoreboard = ui->sprites[UI_SPRITE_SCOREBOARD];

    gfx_detach_rdp();

    const int center_x = (gfx->width >> 1);
    const int x = center_x - (scoreboard->width >> 1);

    graphics_draw_sprite_trans( gfx->disp, x, ui->board_y, scoreboard );
}

inline static void ui_medal_draw(const ui_t * ui)
{
    int stride;
    const int score = ui->last_score;
    if (score >= UI_MEDAL_SCORE_PLATINUM) stride = UI_MEDAL_STRIDE_PLATINUM;
    else if (score >= UI_MEDAL_SCORE_GOLD) stride = UI_MEDAL_STRIDE_GOLD;
    else if (score >= UI_MEDAL_SCORE_SILVER) stride = UI_MEDAL_STRIDE_SILVER;
    else if (score >= UI_MEDAL_SCORE_BRONZE) stride = UI_MEDAL_STRIDE_BRONZE;
    else return;

    sprite_t * const medal = ui->sprites[UI_SPRITE_MEDAL];

    gfx_detach_rdp();

    const int center_x = (gfx->width >> 1);
    const int center_y = (gfx->height >> 1);
    const int x = center_x - ((medal->width / medal->hslices) >> 1) - 32;
    const int y = center_y - ((medal->height / medal->vslices) >> 1) + 4;

    graphics_draw_sprite_trans_stride( gfx->disp, x, y, medal, stride );
}

inline static void ui_highscores_score_draw(const ui_t * ui, int score, int y)
{
    sprite_t * const font = ui->sprites[UI_SPRITE_FONT_MED];
    const display_context_t disp = gfx->disp;

    size_t i = 0, num_digits;
    int digits[UI_SCORE_MAX_DIGITS] = { 0 };
    do
    {
        digits[i] = score % 10;
        score /= 10;
        num_digits = ++i;
    }
    while (score != 0 && num_digits < UI_SCORE_MAX_DIGITS);

    const int digit_w = font->width / font->hslices;
    const int center_x = gfx->width >> 1;
    int x = center_x + 38;
    for (i = 0; i < num_digits; i++)
    {
        graphics_draw_sprite_trans_stride( disp, x, y, font, digits[i] );
        x -= digit_w;
    }
}

inline static void ui_highscores_draw(const ui_t * ui)
{
    gfx_detach_rdp();

    const int center_x = (gfx->width >> 1);
    const int center_y = (gfx->height >> 1);
    ui_highscores_score_draw( ui, ui->last_score_acc, center_y - 11 );
    ui_highscores_score_draw( ui, ui->high_score_acc, center_y + 10);

    if ( ui->new_high_score && ui->high_score_acc == ui->high_score)
    {
        const int new_x = center_x + 10;
        const int new_y = center_y + 1;
        graphics_draw_sprite( gfx->disp, new_x, new_y, ui->sprites[UI_SPRITE_NEW] );
    }
}

inline static void ui_flash_draw(const ui_t * ui)
{
    gfx_rdp_color_fill();
    rdp_set_primitive_color( ui->flash_color );
    const int bx = gfx->width - 1;
    const int by = gfx->height - 1;
    rdp_draw_filled_rectangle( 0, 0, bx, by );
}

void ui_draw(const ui_t * ui)
{
    if ( ui->flash_draw )
    {
        ui_flash_draw( ui );
        return;
    }
    switch (ui->state)
    {
        case BIRD_STATE_TITLE:
            ui_logo_draw( ui );
            break;
        case BIRD_STATE_READY:
            ui_score_draw( ui );
            ui_heading_draw( ui, UI_HEADING_GET_READY );
            ui_howto_draw( ui );
            break;
        case BIRD_STATE_PLAY:
        case BIRD_STATE_DYING:
            ui_score_draw( ui );
            break;
        case BIRD_STATE_DEAD:
            if ( ui->heading_draw )
            {
                ui_heading_draw( ui, UI_HEADING_GAME_OVER );
            }
            if ( ui->board_draw )
            {
                ui_scoreboard_draw( ui );
            }
            if ( ui->score_draw )
            {
                ui_highscores_draw( ui );
            }
            if ( ui->medal_draw )
            {
                ui_medal_draw( ui );
            }
            break;
    }
}
