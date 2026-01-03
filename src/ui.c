/**
 * FlappyBird-N64 - ui.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include "ui.h"

#include "system.h"
#include "gfx.h"
#include "sfx.h"
#include "bg.h"
#include "bird.h"

/* UI definitions */

// This should be set by the project Makefile
#ifndef ROM_VERSION
#define ROM_VERSION ""
#endif

#define UI_SCORE_MAX_DIGITS     ((size_t)5)

#define UI_DEATH_FLASH_TICKS    ((int)80 * TICKS_PER_MS)
#define UI_DEATH_HEADING_DELAY  ((int)600 * TICKS_PER_MS)
#define UI_DEATH_BOARD_DELAY    ((int)1500 * TICKS_PER_MS)
#define UI_DEATH_BOARD_DY_TICKS ((float)200.0 * TICKS_PER_MS)
#define UI_DEATH_SCORE_DELAY    ((int)48 * TICKS_PER_MS)

static color_t UI_DARK_COLOR  = {0};
static color_t UI_LIGHT_COLOR = {0};
static color_t UI_FLASH_COLOR = {0};

/* Font style IDs for UI text */
#define UI_STYLE_SHADOW 1
#define UI_STYLE_TEXT   2

static inline void ui_init_colors(void)
{
    UI_DARK_COLOR  = RGBA32(0x57, 0x37, 0x47, 0xFF);
    UI_LIGHT_COLOR = RGBA32(0xFF, 0xFF, 0xFF, 0xFF);
    UI_FLASH_COLOR = RGBA32(0xFF, 0xFF, 0xFF, 0xFF);
}

typedef enum
{
    UI_HEADING_GET_READY,
    UI_HEADING_GAME_OVER,
} ui_heading_t;

typedef enum
{
    UI_MEDAL_SCORE_BRONZE = 10,
    UI_MEDAL_SCORE_SILVER = 20,
    UI_MEDAL_SCORE_GOLD = 30,
    UI_MEDAL_SCORE_PLATINUM = 40,
} ui_medal_score_t;

typedef enum
{
    UI_MEDAL_STRIDE_BRONZE,
    UI_MEDAL_STRIDE_SILVER,
    UI_MEDAL_STRIDE_GOLD,
    UI_MEDAL_STRIDE_PLATINUM,
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
    // Additional sprites go above this line
    UI_SPRITES_COUNT, // Not an actual sprite, just a handy counter
} ui_sprite_t;

// This array must line up with ui_sprite_t
static const char *const UI_SPRITE_FILES[UI_SPRITES_COUNT] = {
    "gfx/logo.sprite",
    "gfx/headings.sprite",
    "gfx/how-to.sprite",
    "gfx/scoreboard.sprite",
    "gfx/medal.sprite",
    "gfx/font-large.sprite",
    "gfx/font-medium.sprite",
    "gfx/new.sprite",
    "gfx/sparkle.sprite",
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
    color_t text_color;
    color_t shadow_color;
    sprite_t *sprites[UI_SPRITES_COUNT];
    /* Death */
    bool did_flash;
    bool flash_draw;
    color_t flash_color;
    uint32_t hit_ticks;
    /* Game Over */
    uint32_t dead_ticks;
    bool did_gameover;
    bool heading_draw;
    bool board_draw;
    bool score_draw;
    bool medal_draw;
    /* Scoreboard animations */
    uint32_t board_ticks;
    int board_y;
    uint32_t score_ticks;
    int last_score_acc;
    int high_score_acc;
} ui_t;

/* UI implementation */

static void ui_set_time_mode(ui_t *ui, bg_time_mode_t time_mode)
{
    ui->time_mode = time_mode;
    if (time_mode == BG_TIME_DAY)
    {
        ui->text_color = UI_DARK_COLOR;
        ui->shadow_color = UI_LIGHT_COLOR;
    }
    else if (time_mode == BG_TIME_NIGHT)
    {
        ui->text_color = UI_LIGHT_COLOR;
        ui->shadow_color = UI_DARK_COLOR;
    }
    /* Update font styles for current time mode */
    rdpq_font_t *font = (rdpq_font_t *)rdpq_text_get_font(FONT_DEBUG);
    if (font)
    {
        rdpq_font_style(font, UI_STYLE_SHADOW, &(rdpq_fontstyle_t){ .color = ui->shadow_color });
        rdpq_font_style(font, UI_STYLE_TEXT, &(rdpq_fontstyle_t){ .color = ui->text_color });
    }
}

ui_t *ui_init(void)
{
    ui_init_colors();
    ui_t *ui = malloc(sizeof(ui_t));
    if (ui == NULL) return NULL;
    memset(ui, 0, sizeof(ui_t));

    ui->flash_color = UI_FLASH_COLOR;
    ui->board_y = gfx->height;
    ui_set_time_mode(ui, bg_get_time_mode());
    // Load the sprites
    for (size_t i = 0; i < UI_SPRITES_COUNT; i++)
    {
        ui->sprites[i] = read_dfs_sprite(UI_SPRITE_FILES[i]);
    }
    return ui;
}

void ui_free(ui_t *ui)
{
    for (size_t i = 0; i < UI_SPRITES_COUNT; i++)
    {
        free(ui->sprites[i]);
        ui->sprites[i] = NULL;
    }
    free(ui);
}

static void ui_bird_tick(ui_t *ui, const bird_t *bird)
{
    /* Synchronize bird state to UI */
    ui->state = bird->state;
    switch (ui->state)
    {
    case BIRD_STATE_DEAD:
        ui->dead_ticks = bird->dead_ticks;
    case BIRD_STATE_DYING:
        ui->hit_ticks = bird->hit_ticks;
        break;
    default:
        break;
    }
    /* High scoring */
    ui->last_score = bird->score;
    if (bird->score == 0)
    {
        ui->new_high_score = false;
    }
    if (bird->score > ui->high_score)
    {
        ui->high_score = bird->score;
        ui->new_high_score = true;
    }
}

static void ui_flash_tick(ui_t *ui)
{
    const uint32_t now_ticks = TICKS_READ();
    /* Flash the screen for a split second after the bird dies */
    if (ui->state == BIRD_STATE_DYING ||
        ui->state == BIRD_STATE_DEAD)
    {
        if (!ui->did_flash)
        {
            const bool was_flash_draw = ui->flash_draw;
            ui->flash_draw = (
                TICKS_DISTANCE(ui->hit_ticks, now_ticks)
                <= UI_DEATH_FLASH_TICKS
            );
            if (was_flash_draw && !ui->flash_draw)
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

static void ui_gameover_tick(ui_t *ui)
{
    if (ui->state != BIRD_STATE_DEAD)
    {
        ui->did_gameover = false;
        return;
    }
    if (ui->did_gameover)
    {
        // TODO Medal sparkles
        return;
    }
    /* Animate the Game Over UI */
    const uint32_t now_ticks = TICKS_READ();
    const int dead_diff_ticks = TICKS_DISTANCE(ui->dead_ticks, now_ticks);
    /* Only show the scores and medal after the scoreboard appears */
    ui->score_draw = false;
    ui->medal_draw = false;
    /* Show the game over heading and play a sound */
    const bool was_heading_draw = ui->heading_draw;
    ui->heading_draw = dead_diff_ticks >= UI_DEATH_HEADING_DELAY;
    if (!was_heading_draw && ui->heading_draw)
    {
        sfx_play(SFX_SWOOSH);
    }
    /* Show the scoreboard and play a sound */
    const bool was_board_draw = ui->board_draw;
    ui->board_draw = dead_diff_ticks >= UI_DEATH_BOARD_DELAY;
    if (!was_board_draw && ui->board_draw)
    {
        sfx_play(SFX_SWOOSH);
        ui->board_ticks = now_ticks;
    }
    if (ui->board_draw)
    {
        sprite_t *const scoreboard = ui->sprites[UI_SPRITE_SCOREBOARD];
        const int board_diff_ticks = TICKS_DISTANCE(ui->board_ticks, now_ticks);
        const int max_y = display_get_height();
        const int center_y = max_y / 2;
        const int min_y = center_y - (scoreboard->height / 2);
        ui->score_draw = board_diff_ticks >= UI_DEATH_BOARD_DY_TICKS;
        if (!ui->score_draw)
        {
            /* Pop the scoreboard up from the bottom */
            const int y_diff = max_y - min_y;
            const float y_factor = board_diff_ticks / UI_DEATH_BOARD_DY_TICKS;
            if (y_factor >= 0.0 && y_factor < 1.0)
            {
                ui->board_y = max_y - (y_diff * y_factor);
            }
            else
            {
                ui->board_y = max_y - y_diff;
            }
            /* Reset the score accumulator */
            ui->score_ticks = now_ticks;
            ui->last_score_acc = 0;
        }
        else
        {
            ui->board_y = min_y;
        }
    }
    if (ui->score_draw)
    {
        if (ui->last_score_acc < ui->last_score ||
            ui->high_score_acc < ui->high_score)
        {
            const int score_diff_ticks = TICKS_DISTANCE(ui->score_ticks, now_ticks);
            if (score_diff_ticks >= UI_DEATH_SCORE_DELAY)
            {
                if (ui->last_score_acc < ui->last_score)
                {
                    ui->last_score_acc++;
                }
                else if (ui->high_score_acc < ui->high_score)
                {
                    ui->high_score_acc++;
                }
                ui->score_ticks = now_ticks;
            }
        }
        else
        {
            ui->medal_draw = true;
            ui->did_gameover = true;
        }
    }
}

void ui_tick(ui_t *ui, const bird_t *bird)
{
    /* Synchronize background state to UI */
    const bg_time_mode_t bg_time_mode = bg_get_time_mode();
    if (ui->time_mode != bg_time_mode)
    {
        ui_set_time_mode(ui, bg_time_mode);
    }
    ui_bird_tick(ui, bird);
    ui_flash_tick(ui);
    ui_gameover_tick(ui);
}

/* Below this point there be magic numbers! */

static void ui_logo_draw(const ui_t *ui)
{
    sprite_t *const logo = ui->sprites[UI_SPRITE_LOGO];

    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    const int logo_x = center_x - (logo->width / 2);
    const int logo_y = center_y - (logo->height * 3.5);

    /* Draw logo sprite */
    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(logo, logo_x, logo_y, NULL);

    const char *const credit1_str = "Game by .GEARS";
    const int credit1_w = strlen(credit1_str) * 8;
    const int credit1_x = center_x - (credit1_w / 2);
    const int credit1_y = gfx->height - 80;

    const char *const credit2_str = "N64 Port by Meeq";
    const int credit2_w = strlen(credit2_str) * 8;
    const int credit2_x = center_x - (credit2_w / 2);
    const int credit2_y = gfx->height - 62;

    const char *const version_str = ROM_VERSION;
    const int version_w = strlen(version_str) * 8;
    const int version_x = gfx->width - 32 - version_w;
    const int version_y = gfx->height - 32;

    /* Draw a shadow under the text */
    rdpq_textparms_t shadow_parms = { .style_id = UI_STYLE_SHADOW };
    rdpq_text_print(&shadow_parms, FONT_DEBUG, credit1_x + 1, credit1_y + 1, credit1_str);
    rdpq_text_print(&shadow_parms, FONT_DEBUG, credit2_x + 1, credit2_y + 1, credit2_str);
    if (version_w)
    {
        rdpq_text_print(&shadow_parms, FONT_DEBUG, version_x + 1, version_y + 1, version_str);
    }

    /* Draw the same text on top of the shadow */
    rdpq_textparms_t text_parms = { .style_id = UI_STYLE_TEXT };
    rdpq_text_print(&text_parms, FONT_DEBUG, credit1_x, credit1_y, credit1_str);
    rdpq_text_print(&text_parms, FONT_DEBUG, credit2_x, credit2_y, credit2_str);
    if (version_w)
    {
        rdpq_text_print(&text_parms, FONT_DEBUG, version_x, version_y, version_str);
    }
}

static void ui_heading_draw(const ui_t *ui, int stride)
{
    sprite_t *const headings = ui->sprites[UI_SPRITE_HEADINGS];

    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    const int x = center_x - (headings->width / 2);
    const int y = center_y - 70;

    /* Calculate slice dimensions for strided sprite */
    const int slice_h = headings->height / headings->vslices;
    const int t_offset = stride * slice_h;

    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(headings, x, y, &(rdpq_blitparms_t){
        .t0 = t_offset,
        .height = slice_h,
    });
}

static void ui_howto_draw(const ui_t *ui)
{
    sprite_t *const howto = ui->sprites[UI_SPRITE_HOWTO];

    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    const int x = center_x - (howto->width / 2);
    const int y = center_y - (howto->height / 1.45);

    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(howto, x, y, NULL);
}

static void ui_score_draw(const ui_t *ui)
{
    uint16_t score = ui->last_score;
    sprite_t *const font = ui->sprites[UI_SPRITE_FONT_LARGE];

    size_t i = 0, num_digits;
    int digits[UI_SCORE_MAX_DIGITS];
    do
    {
        digits[i] = score % 10;
        score /= 10;
        num_digits = ++i;
    } while (score != 0 && num_digits < UI_SCORE_MAX_DIGITS);

    const int digit_w = font->width / font->hslices;
    const int digit_h = font->height / font->vslices;
    const int score_w = digit_w * num_digits;
    const int center_x = gfx->width / 2;
    const int y = 20;

    rdpq_set_mode_copy(true);

    int x = center_x + (score_w / 2) - digit_w;
    for (i = 0; i < num_digits; i++)
    {
        const int s_offset = digits[i] * digit_w;
        rdpq_sprite_blit(font, x, y, &(rdpq_blitparms_t){
            .s0 = s_offset,
            .width = digit_w,
            .height = digit_h,
        });
        x -= digit_w;
    }
}

static void ui_scoreboard_draw(const ui_t *ui)
{
    sprite_t *const scoreboard = ui->sprites[UI_SPRITE_SCOREBOARD];

    const int center_x = (gfx->width / 2);
    const int x = center_x - (scoreboard->width / 2);

    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(scoreboard, x, ui->board_y, NULL);
}

static void ui_medal_draw(const ui_t *ui)
{
    int stride;
    const int score = ui->last_score;
    if (score >= UI_MEDAL_SCORE_PLATINUM)
        stride = UI_MEDAL_STRIDE_PLATINUM;
    else if (score >= UI_MEDAL_SCORE_GOLD)
        stride = UI_MEDAL_STRIDE_GOLD;
    else if (score >= UI_MEDAL_SCORE_SILVER)
        stride = UI_MEDAL_STRIDE_SILVER;
    else if (score >= UI_MEDAL_SCORE_BRONZE)
        stride = UI_MEDAL_STRIDE_BRONZE;
    else
        return;

    sprite_t *const medal = ui->sprites[UI_SPRITE_MEDAL];

    const int slice_w = medal->width / medal->hslices;
    const int slice_h = medal->height / medal->vslices;
    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    const int x = center_x - (slice_w / 2) - 32;
    const int y = center_y - (slice_h / 2) + 4;

    /* Calculate texture offset based on stride (horizontal slices) */
    const int s_offset = stride * slice_w;

    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(medal, x, y, &(rdpq_blitparms_t){
        .s0 = s_offset,
        .width = slice_w,
        .height = slice_h,
    });
}

static void ui_highscores_score_draw(const ui_t *ui, int score, int y)
{
    sprite_t *const font = ui->sprites[UI_SPRITE_FONT_MED];

    size_t i = 0, num_digits;
    int digits[UI_SCORE_MAX_DIGITS] = {0};
    do
    {
        digits[i] = score % 10;
        score /= 10;
        num_digits = ++i;
    } while (score != 0 && num_digits < UI_SCORE_MAX_DIGITS);

    const int digit_w = font->width / font->hslices;
    const int digit_h = font->height / font->vslices;
    const int center_x = gfx->width / 2;

    rdpq_set_mode_copy(true);

    int x = center_x + 38;
    for (i = 0; i < num_digits; i++)
    {
        const int s_offset = digits[i] * digit_w;
        rdpq_sprite_blit(font, x, y, &(rdpq_blitparms_t){
            .s0 = s_offset,
            .width = digit_w,
            .height = digit_h,
        });
        x -= digit_w;
    }
}

static void ui_highscores_draw(const ui_t *ui)
{
    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    ui_highscores_score_draw(ui, ui->last_score_acc, center_y - 11);
    ui_highscores_score_draw(ui, ui->high_score_acc, center_y + 10);

    if (ui->new_high_score && ui->high_score_acc == ui->high_score)
    {
        sprite_t *const new_sprite = ui->sprites[UI_SPRITE_NEW];
        const int new_x = center_x + 10;
        const int new_y = center_y + 1;

        rdpq_set_mode_copy(true);
        rdpq_sprite_blit(new_sprite, new_x, new_y, NULL);
    }
}

static void ui_flash_draw(const ui_t *ui)
{
    gfx_rdp_color_fill(ui->flash_color);
    rdpq_fill_rectangle(0, 0, gfx->width, gfx->height);
}

void ui_draw(const ui_t *ui)
{
    if (ui->flash_draw)
    {
        ui_flash_draw(ui);
        return;
    }
    switch (ui->state)
    {
    case BIRD_STATE_TITLE:
        ui_logo_draw(ui);
        break;
    case BIRD_STATE_READY:
        ui_score_draw(ui);
        ui_heading_draw(ui, UI_HEADING_GET_READY);
        ui_howto_draw(ui);
        break;
    case BIRD_STATE_PLAY:
    case BIRD_STATE_DYING:
        ui_score_draw(ui);
        break;
    case BIRD_STATE_DEAD:
        if (ui->heading_draw)
        {
            ui_heading_draw(ui, UI_HEADING_GAME_OVER);
        }
        if (ui->board_draw)
        {
            ui_scoreboard_draw(ui);
        }
        if (ui->score_draw)
        {
            ui_highscores_draw(ui);
        }
        if (ui->medal_draw)
        {
            ui_medal_draw(ui);
        }
        break;
    }
}
