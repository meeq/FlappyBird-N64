/**
 * FlappyBird-N64 - bg.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include "bg.h"

#include "system.h"
#include "gfx.h"

/* Background constants */

#define BG_SCROLL_RATE          (15 * TICKS_PER_MS)

#define BG_SKY_SCROLL_DX        ((float) -0.008)
#define BG_CITY_SCROLL_DX       ((float) -0.04)
#define BG_HILL_SCROLL_DX       ((float) -0.2)
#define BG_GROUND_SCROLL_DX     ((float) -1.0)

#define BG_SKY_FILL_Y           ((int) 0)
#define BG_SKY_FILL_H           ((int) 141)
#define BG_CLOUD_TOP_Y          ((int) 130)
#define BG_CLOUD_FILL_Y         ((int) 140)
#define BG_CLOUD_FILL_H         ((int) 32)
#define BG_CITY_TOP_Y           ((int) 157)
#define BG_HILL_TOP_Y           ((int) 170)
#define BG_HILL_FILL_Y          ((int) 178)
#define BG_HILL_FILL_H          ((int) 12)
//      BG_GROUND_TOP_Y         (defined in background.h)
#define BG_GROUND_FILL_Y        ((int) 200)
#define BG_GROUND_FILL_H        ((int) 40)

static color_t BG_COLOR_DAY_SKY     = {0};
static color_t BG_COLOR_NIGHT_SKY   = {0};
static color_t BG_COLOR_DAY_CLOUD   = {0};
static color_t BG_COLOR_NIGHT_CLOUD = {0};
static color_t BG_COLOR_DAY_HILL    = {0};
static color_t BG_COLOR_NIGHT_HILL  = {0};
static color_t BG_COLOR_GROUND      = {0};

static inline void bg_init_colors(void)
{
    BG_COLOR_DAY_SKY     = RGBA32(0x4E, 0xC0, 0xCA, 0xFF);
    BG_COLOR_NIGHT_SKY   = RGBA32(0x00, 0x87, 0x93, 0xFF);
    BG_COLOR_DAY_CLOUD   = RGBA32(0xE4, 0xFD, 0xD0, 0xFF);
    BG_COLOR_NIGHT_CLOUD = RGBA32(0x15, 0xA5, 0xB5, 0xFF);
    BG_COLOR_DAY_HILL    = RGBA32(0x52, 0xE0, 0x5D, 0xFF);
    BG_COLOR_NIGHT_HILL  = RGBA32(0x14, 0x96, 0x02, 0xFF);
    BG_COLOR_GROUND      = RGBA32(0xDF, 0xD8, 0x93, 0xFF);
}

/* Background types */

typedef enum
{
    BG_SPRITE_CLOUD_DAY,
    BG_SPRITE_CLOUD_NIGHT,
    BG_SPRITE_CITY_DAY,
    BG_SPRITE_CITY_NIGHT,
    BG_SPRITE_HILL_DAY,
    BG_SPRITE_HILL_NIGHT,
    BG_SPRITE_GROUND,
    // Additional sprites go above this line
    BG_SPRITES_COUNT, // Not an actual sprite, just a handy counter
} bg_sprite_t;

// This array must line up with bg_sprite_t
static const char *const BG_SPRITE_FILES[BG_SPRITES_COUNT] = {
    "gfx/bg-cloud-day.sprite",
    "gfx/bg-cloud-night.sprite",
    "gfx/bg-city-day.sprite",
    "gfx/bg-city-night.sprite",
    "gfx/bg-hill-day.sprite",
    "gfx/bg-hill-night.sprite",
    "gfx/ground.sprite",
};

typedef struct bg_fill_color_s
{
    color_t color;
    int y;
    int h;
} bg_fill_color_t;

typedef struct bg_fill_sprite_s
{
    bg_sprite_t sprite;
    int y;
    float scroll_x;
    int scroll_w;
    float scroll_dx;
} bg_fill_sprite_t;

static struct background_s
{
    bool initialized;
    sprite_t *sprites[BG_SPRITES_COUNT];
    // Setup state
    bg_time_mode_t time_mode;
    uint64_t scroll_ticks;
    // Color fills
    bg_fill_color_t sky_fill;
    bg_fill_color_t cloud_fill;
    bg_fill_color_t hill_fill;
    bg_fill_color_t ground_fill;
    // Texture fills
    bg_fill_sprite_t cloud_top;
    bg_fill_sprite_t city;
    bg_fill_sprite_t hill_top;
    bg_fill_sprite_t ground_top;
} bg = {0};

/* Background implementation */

void bg_init(void)
{
    assert(!bg.initialized);
    bg_init_colors();
    memset(&bg, 0, sizeof bg);
    bg.initialized = true;
    for (size_t i = 0; i < BG_SPRITES_COUNT; i++)
    {
        bg.sprites[i] = read_dfs_sprite(BG_SPRITE_FILES[i]);
    }
    bg.sky_fill = (bg_fill_color_t){
        .y = BG_SKY_FILL_Y,
        .h = BG_SKY_FILL_H,
    };
    bg.cloud_top = (bg_fill_sprite_t){
        .y = BG_CLOUD_TOP_Y,
        .scroll_x = 0,
        .scroll_dx = BG_SKY_SCROLL_DX,
        .scroll_w = bg.sprites[BG_SPRITE_CLOUD_DAY]->width,
    };
    bg.cloud_fill = (bg_fill_color_t){
        .y = BG_CLOUD_FILL_Y,
        .h = BG_CLOUD_FILL_H,
    };
    bg.city = (bg_fill_sprite_t){
        .y = BG_CITY_TOP_Y,
        .scroll_x = 0,
        .scroll_dx = BG_CITY_SCROLL_DX,
        .scroll_w = bg.sprites[BG_SPRITE_CITY_DAY]->width,
    };
    bg.hill_top = (bg_fill_sprite_t){
        .y = BG_HILL_TOP_Y,
        .scroll_x = 0,
        .scroll_dx = BG_HILL_SCROLL_DX,
        .scroll_w = bg.sprites[BG_SPRITE_HILL_DAY]->width,
    };
    bg.hill_fill = (bg_fill_color_t){
        .y = BG_HILL_FILL_Y, .h = BG_HILL_FILL_H};
    bg.ground_top = (bg_fill_sprite_t){
        .sprite = BG_SPRITE_GROUND,
        .y = BG_GROUND_TOP_Y,
        .scroll_x = 0,
        .scroll_dx = BG_GROUND_SCROLL_DX,
        .scroll_w = bg.sprites[BG_SPRITE_GROUND]->width,
    };
    bg.ground_fill = (bg_fill_color_t){
        .color = BG_COLOR_GROUND,
        .y = BG_GROUND_FILL_Y,
        .h = BG_GROUND_FILL_H,
    };
    bg_set_time_mode(BG_TIME_DAY);
}

bg_time_mode_t bg_get_time_mode(void)
{
    return bg.time_mode;
}

void bg_set_time_mode(bg_time_mode_t time_mode)
{
    bg.time_mode = time_mode;
    if (time_mode == BG_TIME_DAY)
    {
        bg.sky_fill.color = BG_COLOR_DAY_SKY;
        bg.cloud_fill.color = BG_COLOR_DAY_CLOUD;
        bg.hill_fill.color = BG_COLOR_DAY_HILL;
        bg.cloud_top.sprite = BG_SPRITE_CLOUD_DAY;
        bg.city.sprite = BG_SPRITE_CITY_DAY;
        bg.hill_top.sprite = BG_SPRITE_HILL_DAY;
    }
    else if (time_mode == BG_TIME_NIGHT)
    {
        bg.sky_fill.color = BG_COLOR_NIGHT_SKY;
        bg.cloud_fill.color = BG_COLOR_NIGHT_CLOUD;
        bg.hill_fill.color = BG_COLOR_NIGHT_HILL;
        bg.cloud_top.sprite = BG_SPRITE_CLOUD_NIGHT;
        bg.city.sprite = BG_SPRITE_CITY_NIGHT;
        bg.hill_top.sprite = BG_SPRITE_HILL_NIGHT;
    }
}

static inline bg_time_mode_t bg_random_time_mode(void)
{
    return ((float)rand() / (float)RAND_MAX) * BG_TIME_MODES_COUNT;
}

void bg_randomize_time_mode(void)
{
    bg_set_time_mode(bg_random_time_mode());
}

void bg_tick_scroll(bg_fill_sprite_t *fill)
{
    float x = fill->scroll_x;
    const int w = fill->scroll_w;
    x += fill->scroll_dx;
    while (x > w) x -= w;
    while (x < -w) x += w;
    fill->scroll_x = x;
}

void bg_tick(const joypad_buttons_t *buttons)
{
    /* Switch between day and night */
    if (buttons->l)
    {
        bg_set_time_mode(!bg.time_mode);
    }
    /* Scroll the bg */
    const uint64_t now_ticks = get_ticks();
    if ((now_ticks - bg.scroll_ticks) >= BG_SCROLL_RATE)
    {
        bg.scroll_ticks = now_ticks;
        bg_tick_scroll(&bg.cloud_top);
        bg_tick_scroll(&bg.city);
        bg_tick_scroll(&bg.hill_top);
        bg_tick_scroll(&bg.ground_top);
    }
}

static void bg_draw_color(const bg_fill_color_t * const fill)
{
    rdpq_set_mode_fill(fill->color);
    const int tx = 0, ty = GFX_SCALE(fill->y);
    const int bx = gfx->width, by = GFX_SCALE(fill->y + fill->h);
    rdpq_fill_rectangle(tx, ty, bx, by);
}

static void bg_draw_sprite(const bg_fill_sprite_t *const fill)
{
    sprite_t *sprite = bg.sprites[fill->sprite];
    assert(sprite != NULL);
    assert(sprite->hslices == 1);
    assert(sprite->vslices == 1);

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);

    /* Texture coordinates (unscaled) */
    const float scroll_x = fill->scroll_x;
    const int tex_h = sprite->height;

    /* Screen coordinates (scaled) */
    const int scr_ty = GFX_SCALE(fill->y);
    const int scr_by = scr_ty + GFX_SCALE(tex_h);
    const int scr_max_w = gfx->width;

    /* Upload sprite with horizontal tiling */
    rdpq_texparms_t tiled_texparams = {
        .s = { .repeats = REPEAT_INFINITE, .mirror = MIRROR_NONE },
        .t = { .repeats = 1, .mirror = MIRROR_NONE },
    };
    rdpq_sprite_upload(TILE0, sprite, &tiled_texparams);

    /* Calculate screen X start based on scroll, handling wrap */
    int scr_tx = GFX_SCALE(scroll_x);
    float tex_s0 = 0;
    if (scr_tx < 0) {
        /* Left edge clipped - adjust texture start coordinate */
        tex_s0 = -scroll_x;
        scr_tx = 0;
    }

    /* Draw with hardware tiling - texture repeats automatically */
    float tex_s1 = tex_s0 + (scr_max_w - scr_tx) / gfx->scale;
    rdpq_texture_rectangle_scaled(TILE0,
        scr_tx, scr_ty, scr_max_w, scr_by,
        tex_s0, 0, tex_s1, tex_h);
}

void bg_draw_sky(void)
{
    /* Color fills (sky, clouds, hills - but not ground) */
    bg_draw_color(&bg.sky_fill);
    bg_draw_color(&bg.cloud_fill);
    bg_draw_color(&bg.hill_fill);

    /* Texture fills (clouds, city, hills - but not ground) */
    bg_draw_sprite(&bg.cloud_top);
    bg_draw_sprite(&bg.city);
    bg_draw_sprite(&bg.hill_top);
}

void bg_draw_ground(void)
{
    /* Ground is drawn separately so it can cover pipes/bird */
    bg_draw_color(&bg.ground_fill);
    bg_draw_sprite(&bg.ground_top);
}
