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

#include <eeprom.h>

/* UI definitions */

// This should be set by the project Makefile
#ifndef ROM_VERSION
#define ROM_VERSION ""
#endif

#define UI_SCORE_MAX_DIGITS     ((size_t)5)

#define UI_DEATH_FLASH_TICKS    ((uint64_t)150 * TICKS_PER_MS)
#define UI_DEATH_HEADING_DELAY  ((int)600 * TICKS_PER_MS)
#define UI_DEATH_BOARD_DELAY    ((int)1500 * TICKS_PER_MS)
#define UI_DEATH_BOARD_DY_TICKS ((float)200.0 * TICKS_PER_MS)
#define UI_DEATH_SCORE_DELAY    ((int)48 * TICKS_PER_MS)
#define UI_SPARKLE_CYCLE_TICKS  ((int)500 * TICKS_PER_MS)

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
    UI_SPRITE_LOGO,
    UI_SPRITE_HEADINGS,
    UI_SPRITE_HOWTO,
    UI_SPRITE_SCOREBOARD,
    UI_SPRITE_MEDAL_BRONZE,
    UI_SPRITE_MEDAL_SILVER,
    UI_SPRITE_MEDAL_GOLD,
    UI_SPRITE_MEDAL_PLATINUM,
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
    "gfx/medal-bronze.sprite",
    "gfx/medal-silver.sprite",
    "gfx/medal-gold.sprite",
    "gfx/medal-platinum.sprite",
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
    uint8_t flash_alpha;
    color_t flash_color;
    uint64_t hit_ticks;
    /* Game Over */
    uint64_t dead_ticks;
    bool did_gameover;
    bool heading_draw;
    bool board_draw;
    bool score_draw;
    bool medal_draw;
    /* Scoreboard animations */
    uint64_t board_ticks;
    float board_y_factor;  /* 0.0 = final position, 1.0 = off screen at bottom */
    uint64_t score_ticks;
    int last_score_acc;
    /* Medal sparkle animation */
    uint64_t sparkle_ticks;
    int sparkle_x;
    int sparkle_y;
} ui_t;

/* Forward declarations */
static void ui_randomize_sparkle_position(ui_t *ui);

/* EEPROM high score persistence */

#define EEPROM_MAGIC 0x464C4150  /* "FLAP" in ASCII */

static void ui_load_high_score(ui_t *ui)
{
    eeprom_type_t eeprom_type = eeprom_present();
    if (eeprom_type == EEPROM_NONE)
    {
        debugf("[EEPROM] No EEPROM detected\n");
        return;
    }
    debugf("[EEPROM] Detected EEPROM type: %s\n", eeprom_type == EEPROM_4K ? "4K" : "16K");

    uint8_t data[EEPROM_BLOCK_SIZE];
    eeprom_read(0, data);

    /* Check magic signature */
    uint32_t magic = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    if (magic != EEPROM_MAGIC)
    {
        debugf("[EEPROM] No valid save found (magic: 0x%08lX)\n", magic);
        return;
    }

    /* Extract high score */
    ui->high_score = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    debugf("[EEPROM] Loaded high score: %d\n", ui->high_score);
}

static void ui_save_high_score(const ui_t *ui)
{
    if (eeprom_present() == EEPROM_NONE) return;

    uint8_t data[EEPROM_BLOCK_SIZE];

    /* Write magic signature */
    data[0] = (EEPROM_MAGIC >> 24) & 0xFF;
    data[1] = (EEPROM_MAGIC >> 16) & 0xFF;
    data[2] = (EEPROM_MAGIC >> 8) & 0xFF;
    data[3] = EEPROM_MAGIC & 0xFF;

    /* Write high score */
    data[4] = (ui->high_score >> 24) & 0xFF;
    data[5] = (ui->high_score >> 16) & 0xFF;
    data[6] = (ui->high_score >> 8) & 0xFF;
    data[7] = ui->high_score & 0xFF;

    eeprom_write(0, data);
    debugf("[EEPROM] Saved high score: %d\n", ui->high_score);
}

/* UI implementation */

static void ui_set_time_mode(ui_t *ui, bg_time_mode_t time_mode)
{
    ui->time_mode = time_mode;
    if (time_mode == BG_TIME_DAY)
    {
        ui->text_color = UI_LIGHT_COLOR;
        ui->shadow_color = UI_DARK_COLOR;
    }
    else if (time_mode == BG_TIME_NIGHT)
    {
        ui->text_color = UI_DARK_COLOR;
        ui->shadow_color = UI_LIGHT_COLOR;
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
    ui->board_y_factor = 1.0f;  /* Start off-screen */
    ui_set_time_mode(ui, bg_get_time_mode());
    ui_load_high_score(ui);
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
    const uint64_t now_ticks = get_ticks();
    /* Flash the screen for a split second after the bird dies */
    if (ui->state == BIRD_STATE_DYING ||
        ui->state == BIRD_STATE_DEAD)
    {
        if (!ui->did_flash)
        {
            const uint64_t elapsed = now_ticks - ui->hit_ticks;
            const uint64_t half = UI_DEATH_FLASH_TICKS / 2;
            if (elapsed < half)
            {
                /* Fade in */
                ui->flash_alpha = (elapsed * 255) / half;
            }
            else if (elapsed < UI_DEATH_FLASH_TICKS)
            {
                /* Fade out */
                ui->flash_alpha = 255 - ((elapsed - half) * 255) / half;
            }
            else
            {
                ui->flash_alpha = 0;
                ui->did_flash = true;
            }
            ui->flash_draw = (ui->flash_alpha > 0);
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
        /* Medal sparkle animation - pick new random position each cycle */
        if (ui->medal_draw)
        {
            const uint64_t now_ticks = get_ticks();
            if ((now_ticks - ui->sparkle_ticks) >= UI_SPARKLE_CYCLE_TICKS)
            {
                ui_randomize_sparkle_position(ui);
                ui->sparkle_ticks = now_ticks;
            }
        }
        return;
    }
    /* Animate the Game Over UI */
    const uint64_t now_ticks = get_ticks();
    const uint64_t dead_diff_ticks = now_ticks - ui->dead_ticks;
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
        const uint64_t board_diff_ticks = now_ticks - ui->board_ticks;
        ui->score_draw = board_diff_ticks >= UI_DEATH_BOARD_DY_TICKS;
        if (!ui->score_draw)
        {
            /* Pop the scoreboard up from the bottom (1.0 = off screen, 0.0 = final pos) */
            float y_factor = 1.0f - (board_diff_ticks / UI_DEATH_BOARD_DY_TICKS);
            if (y_factor < 0.0f) y_factor = 0.0f;
            ui->board_y_factor = y_factor;
            /* Reset the score accumulator */
            ui->score_ticks = now_ticks;
            ui->last_score_acc = 0;
        }
        else
        {
            ui->board_y_factor = 0.0f;  /* Final position */
        }
    }
    if (ui->score_draw)
    {
        if (ui->last_score_acc < ui->last_score)
        {
            const uint64_t score_diff_ticks = now_ticks - ui->score_ticks;
            if (score_diff_ticks >= UI_DEATH_SCORE_DELAY)
            {
                ui->last_score_acc++;
                ui->score_ticks = now_ticks;
            }
        }
        else
        {
            ui->medal_draw = true;
            ui->did_gameover = true;
            /* Initialize sparkle animation */
            ui_randomize_sparkle_position(ui);
            ui->sparkle_ticks = now_ticks;
            if (ui->new_high_score)
            {
                ui_save_high_score(ui);
            }
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
    const int logo_x = center_x - GFX_SCALE(logo->width / 2);
    const int logo_y = center_y - GFX_SCALE(logo->height * 3.5);

    /* Draw logo sprite */
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_sprite_blit(logo, logo_x, logo_y, &(rdpq_blitparms_t){
        .scale_x = gfx->scale,
        .scale_y = gfx->scale,
    });

    const char *const credit1_str = "Game by .GEARS";
    const int credit1_w = strlen(credit1_str) * 6;
    const int credit1_x = center_x - (credit1_w / 2);
    const int credit1_y = gfx->height - GFX_SCALE(80);

    const char *const credit2_str = "N64 Port by Meeq";
    const int credit2_w = strlen(credit2_str) * 6;
    const int credit2_x = center_x - (credit2_w / 2);
    const int credit2_y = gfx->height - GFX_SCALE(62);

    const char *const version_str = ROM_VERSION;
    const int version_w = strlen(version_str) * 6;
    const int version_x = gfx->width - GFX_SCALE(32) - version_w;
    const int version_y = gfx->height - GFX_SCALE(32);

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
    const int x = center_x - GFX_SCALE(headings->width / 2);
    const int y = center_y - GFX_SCALE(70);

    /* Calculate slice dimensions for strided sprite */
    const int slice_h = headings->height / headings->vslices;
    const int t_offset = stride * slice_h;

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_sprite_blit(headings, x, y, &(rdpq_blitparms_t){
        .t0 = t_offset,
        .height = slice_h,
        .scale_x = gfx->scale,
        .scale_y = gfx->scale,
    });
}

static void ui_howto_draw(const ui_t *ui)
{
    sprite_t *const howto = ui->sprites[UI_SPRITE_HOWTO];

    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    const int x = center_x - GFX_SCALE(howto->width / 2);
    const int y = center_y - GFX_SCALE(howto->height / 1.45);

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_sprite_blit(howto, x, y, &(rdpq_blitparms_t){
        .scale_x = gfx->scale,
        .scale_y = gfx->scale,
    });
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
    const int scaled_digit_w = GFX_SCALE(digit_w);
    const int score_w = scaled_digit_w * num_digits;
    const int center_x = gfx->width / 2;
    const int y = GFX_SCALE(20);

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);

    int x = center_x + (score_w / 2) - scaled_digit_w;
    for (i = 0; i < num_digits; i++)
    {
        const int s_offset = digits[i] * digit_w;
        rdpq_sprite_blit(font, x, y, &(rdpq_blitparms_t){
            .s0 = s_offset,
            .width = digit_w,
            .height = digit_h,
            .scale_x = gfx->scale,
            .scale_y = gfx->scale,
        });
        x -= scaled_digit_w;
    }
}

static void ui_scoreboard_draw(const ui_t *ui)
{
    sprite_t *const scoreboard = ui->sprites[UI_SPRITE_SCOREBOARD];

    const int center_x = (gfx->width / 2);
    const int x = center_x - GFX_SCALE(scoreboard->width / 2);

    /* Compute Y position from normalized factor */
    const int max_y = gfx->height;
    const int center_y = max_y / 2;
    const int min_y = center_y - GFX_SCALE(scoreboard->height / 2);
    const int y_diff = max_y - min_y;
    const int board_y = min_y + (int)(y_diff * ui->board_y_factor);

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_sprite_blit(scoreboard, x, board_y, &(rdpq_blitparms_t){
        .scale_x = gfx->scale,
        .scale_y = gfx->scale,
    });
}

static sprite_t *ui_get_medal_sprite(const ui_t *ui)
{
    const int score = ui->last_score;
    if (score >= UI_MEDAL_SCORE_PLATINUM)
        return ui->sprites[UI_SPRITE_MEDAL_PLATINUM];
    else if (score >= UI_MEDAL_SCORE_GOLD)
        return ui->sprites[UI_SPRITE_MEDAL_GOLD];
    else if (score >= UI_MEDAL_SCORE_SILVER)
        return ui->sprites[UI_SPRITE_MEDAL_SILVER];
    else if (score >= UI_MEDAL_SCORE_BRONZE)
        return ui->sprites[UI_SPRITE_MEDAL_BRONZE];
    else
        return NULL;
}

static void ui_randomize_sparkle_position(ui_t *ui)
{
    sprite_t *const medal = ui_get_medal_sprite(ui);
    sprite_t *const sparkle = ui->sprites[UI_SPRITE_SPARKLE];
    if (medal == NULL) return;

    const int sparkle_w = sparkle->width / sparkle->hslices;
    const int sparkle_h = sparkle->height;
    const int range_x = medal->width - sparkle_w;
    const int range_y = medal->height - sparkle_h;

    ui->sparkle_x = ((float)rand() / (float)RAND_MAX) * range_x;
    ui->sparkle_y = ((float)rand() / (float)RAND_MAX) * range_y;
}

static void ui_medal_draw(const ui_t *ui)
{
    sprite_t *const medal = ui_get_medal_sprite(ui);
    if (medal == NULL)
        return;

    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    const int x = center_x - GFX_SCALE(medal->width / 2) - GFX_SCALE(32);
    const int y = center_y - GFX_SCALE(medal->height / 2) + GFX_SCALE(4);

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_sprite_blit(medal, x, y, &(rdpq_blitparms_t){
        .scale_x = gfx->scale,
        .scale_y = gfx->scale,
    });

    /* Draw sparkle animation */
    sprite_t *const sparkle = ui->sprites[UI_SPRITE_SPARKLE];
    const int64_t now_ticks = get_ticks();
    const int elapsed = now_ticks - ui->sparkle_ticks;

    /* 5 animation phases over 1 second (200ms each): small, medium, large, medium, small */
    int phase = (elapsed * 5) / UI_SPARKLE_CYCLE_TICKS;
    if (phase > 4) phase = 4;
    const int frame_map[] = {0, 1, 2, 1, 0};
    const int frame = frame_map[phase];

    const int sparkle_w = sparkle->width / sparkle->hslices;
    const int sparkle_x = x + GFX_SCALE(ui->sparkle_x);
    const int sparkle_y = y + GFX_SCALE(ui->sparkle_y);

    rdpq_sprite_blit(sparkle, sparkle_x, sparkle_y, &(rdpq_blitparms_t){
        .s0 = frame * sparkle_w,
        .width = sparkle_w,
        .height = sparkle->height,
        .scale_x = gfx->scale,
        .scale_y = gfx->scale,
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
    const int scaled_digit_w = GFX_SCALE(digit_w);
    const int center_x = gfx->width / 2;

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);

    int x = center_x + GFX_SCALE(38);
    for (i = 0; i < num_digits; i++)
    {
        const int s_offset = digits[i] * digit_w;
        rdpq_sprite_blit(font, x, y, &(rdpq_blitparms_t){
            .s0 = s_offset,
            .width = digit_w,
            .height = digit_h,
            .scale_x = gfx->scale,
            .scale_y = gfx->scale,
        });
        x -= scaled_digit_w;
    }
}

static void ui_highscores_draw(const ui_t *ui)
{
    const int center_x = (gfx->width / 2);
    const int center_y = (gfx->height / 2);
    ui_highscores_score_draw(ui, ui->last_score_acc, center_y - GFX_SCALE(11));
    ui_highscores_score_draw(ui, ui->high_score, center_y + GFX_SCALE(10));

    if (ui->new_high_score && ui->last_score_acc == ui->last_score)
    {
        sprite_t *const new_sprite = ui->sprites[UI_SPRITE_NEW];
        const int new_x = center_x + GFX_SCALE(10);
        const int new_y = center_y + GFX_SCALE(1);

        rdpq_set_mode_standard();
        rdpq_mode_alphacompare(1);
        rdpq_sprite_blit(new_sprite, new_x, new_y, &(rdpq_blitparms_t){
            .scale_x = gfx->scale,
            .scale_y = gfx->scale,
        });
    }
}

static void ui_flash_draw(const ui_t *ui)
{
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, ui->flash_alpha));
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
