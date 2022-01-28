/**
 * FlappyBird-N64 - background.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>

#include "background.h"

#include "gfx.h"

/* Background constants */

#define BG_SCROLL_RATE          ((int) 16)

#define BG_SKY_SCROLL_DX        ((float) -0.008)
#define BG_CITY_SCROLL_DX       ((float) -0.04)
#define BG_HILL_SCROLL_DX       ((float) -0.2)
#define BG_GROUND_SCROLL_DX     ((float) -1.0)

#define BG_SKY_FILL_Y           ((int) 0)
#define BG_SKY_FILL_H           ((int) 141)
#define BG_CLOUD_TOP_Y          ((int) 130)
#define BG_CLOUD_FILL_Y         ((int) 141)
#define BG_CLOUD_FILL_H         ((int) 32)
#define BG_CITY_TOP_Y           ((int) 155)
#define BG_HILL_TOP_Y           ((int) 170)
#define BG_HILL_FILL_Y          ((int) 179)
#define BG_HILL_FILL_H          ((int) 11)
//      BG_GROUND_TOP_Y         (defined in background.h)
#define BG_GROUND_FILL_Y        ((int) 200)
#define BG_GROUND_FILL_H        ((int) 40)

#define BG_COLOR_DAY_SKY        graphics_make_color( 0x4E, 0xC0, 0xCA, 0xFF )
#define BG_COLOR_NIGHT_SKY      graphics_make_color( 0x00, 0x87, 0x93, 0xFF )
#define BG_COLOR_DAY_CLOUD      graphics_make_color( 0xE4, 0xFD, 0xD0, 0xFF )
#define BG_COLOR_NIGHT_CLOUD    graphics_make_color( 0x15, 0xA5, 0xB5, 0xFF )
#define BG_COLOR_DAY_HILL       graphics_make_color( 0x52, 0xE0, 0x5D, 0xFF )
#define BG_COLOR_NIGHT_HILL     graphics_make_color( 0x14, 0x96, 0x02, 0xFF )
#define BG_COLOR_GROUND         graphics_make_color( 0xDF, 0xD8, 0x93, 0xFF )

/* Background types */

typedef enum
{
    BG_CLOUD_DAY_SPRITE, BG_CLOUD_NIGHT_SPRITE,
    BG_CITY_DAY_SPRITE,  BG_CITY_NIGHT_SPRITE,
    BG_HILL_DAY_SPRITE,  BG_HILL_NIGHT_SPRITE,
    BG_GROUND_SPRITE,
    BG_NUM_SPRITES,
} bg_sprite_index_t;

static const char * const BG_SPRITE_FILES[BG_NUM_SPRITES] = {
    "/gfx/bg-cloud-day.sprite", "/gfx/bg-cloud-night.sprite",
    "/gfx/bg-city-day.sprite",  "/gfx/bg-city-night.sprite",
    "/gfx/bg-hill-day.sprite",  "/gfx/bg-hill-night.sprite",
    "/gfx/ground.sprite"
};

typedef struct bg_fill_color_s
{
    gfx_color_t color;
    int y;
    int h;
} bg_fill_color_t;

typedef struct bg_fill_sprite_s
{
    bg_sprite_index_t sprite;
    int y;
    float scroll_x;
    int scroll_w;
    float scroll_dx;
} bg_fill_sprite_t;

typedef struct background_s
{
    sprite_t * sprites[BG_NUM_SPRITES];
    // Setup state
    bg_time_mode_t time_mode;
    ticks_t scroll_ms;
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
} background_t;

/* Background implementation */

background_t * background_init(bg_time_mode_t time_mode)
{
    background_t * const bg = malloc( sizeof(background_t) );
    for (size_t i = 0; i < BG_NUM_SPRITES; i++)
    {
        bg->sprites[i] = read_dfs_sprite( BG_SPRITE_FILES[i] );
    }
    bg->scroll_ms = 0;
    bg->sky_fill = (bg_fill_color_t){
        .y = BG_SKY_FILL_Y, .h = BG_SKY_FILL_H,
    };
    bg->cloud_top = (bg_fill_sprite_t){
        .y = BG_CLOUD_TOP_Y,
        .scroll_x = 0, .scroll_dx = BG_SKY_SCROLL_DX,
        .scroll_w = bg->sprites[BG_CLOUD_DAY_SPRITE]->width,
    };
    bg->cloud_fill = (bg_fill_color_t){
        .y = BG_CLOUD_FILL_Y, .h = BG_CLOUD_FILL_H,
    };
    bg->city = (bg_fill_sprite_t){
        .y = BG_CITY_TOP_Y,
        .scroll_x = 0, .scroll_dx = BG_CITY_SCROLL_DX,
        .scroll_w = bg->sprites[BG_CITY_DAY_SPRITE]->width,
    };
    bg->hill_top = (bg_fill_sprite_t){
        .y = BG_HILL_TOP_Y,
        .scroll_x = 0, .scroll_dx = BG_HILL_SCROLL_DX,
        .scroll_w = bg->sprites[BG_HILL_DAY_SPRITE]->width,
    };
    bg->hill_fill = (bg_fill_color_t){
        .y = BG_HILL_FILL_Y, .h = BG_HILL_FILL_H
    };
    bg->ground_top = (bg_fill_sprite_t){
        .sprite = BG_GROUND_SPRITE,
        .y = BG_GROUND_TOP_Y,
        .scroll_x = 0, .scroll_dx = BG_GROUND_SCROLL_DX,
        .scroll_w = bg->sprites[BG_GROUND_SPRITE]->width,
    };
    bg->ground_fill = (bg_fill_color_t){
        .color = BG_COLOR_GROUND,
        .y = BG_GROUND_FILL_Y, .h = BG_GROUND_FILL_H,
    };
    background_set_time_mode( bg, time_mode );
    return bg;
}

bg_time_mode_t background_get_time_mode(const background_t * bg)
{
    return bg->time_mode;
}

void background_set_time_mode(background_t * bg, bg_time_mode_t time_mode)
{
    bg->time_mode = time_mode;
    if ( time_mode == BG_DAY_TIME )
    {
        bg->sky_fill.color = BG_COLOR_DAY_SKY;
        bg->cloud_fill.color = BG_COLOR_DAY_CLOUD;
        bg->hill_fill.color = BG_COLOR_DAY_HILL;
        bg->cloud_top.sprite = BG_CLOUD_DAY_SPRITE;
        bg->city.sprite = BG_CITY_DAY_SPRITE;
        bg->hill_top.sprite = BG_HILL_DAY_SPRITE;
    }
    else
    {
        bg->sky_fill.color = BG_COLOR_NIGHT_SKY;
        bg->cloud_fill.color = BG_COLOR_NIGHT_CLOUD;
        bg->hill_fill.color = BG_COLOR_NIGHT_HILL;
        bg->cloud_top.sprite = BG_CLOUD_NIGHT_SPRITE;
        bg->city.sprite = BG_CITY_NIGHT_SPRITE;
        bg->hill_top.sprite = BG_HILL_NIGHT_SPRITE;
    }
}

inline static bg_time_mode_t background_random_time_mode(void)
{
    return ((float) rand() / (float) RAND_MAX) * BG_NUM_TIME_MODES;
}

void background_randomize_time_mode(background_t * bg)
{
    background_set_time_mode( bg, background_random_time_mode() );
}

void background_free(background_t * bg)
{
    /* Deallocate the sprites */
    for (size_t i = 0; i < BG_NUM_SPRITES; i++)
    {
        free( bg->sprites[i] );
        bg->sprites[i] = NULL;
    }
    free(bg);
}

inline void background_tick_scroll(bg_fill_sprite_t * fill)
{
    float x = fill->scroll_x;
    const int w = fill->scroll_w;
    x += fill->scroll_dx;
    while ( x > w ) x -= w;
    while ( x < -w ) x += w;
    fill->scroll_x = x;
}

void background_tick(background_t * bg, const gamepad_state_t * gamepad)
{
     /* Switch between day and night */
    if( gamepad->L )
    {
        background_set_time_mode( bg, !bg->time_mode );
    }
    /* Scroll the background */
    const ticks_t ticks_ms = get_total_ms();
    if ( ticks_ms - bg->scroll_ms >= BG_SCROLL_RATE )
    {
        bg->scroll_ms = ticks_ms;
        background_tick_scroll( &bg->cloud_top );
        background_tick_scroll( &bg->city );
        background_tick_scroll( &bg->hill_top );
        background_tick_scroll( &bg->ground_top );
    }
}

inline static void background_draw_color(const bg_fill_color_t * const fill)
{
    gfx_rdp_color_fill();
    rdp_set_primitive_color( fill->color );
    const int tx = 0, ty = fill->y;
    const int bx = gfx->width, by = fill->y + fill->h;
    rdp_draw_filled_rectangle( tx, ty, bx, by );
}

void background_draw_sprite(const background_t * const bg,
                            const bg_fill_sprite_t * const fill)
{
    sprite_t * sprite = bg->sprites[fill->sprite];
    if ( sprite == NULL ) return;

    const int scroll_x = fill->scroll_x;
    const int slices = sprite->hslices, max_w = gfx->width;
    const mirror_t mirror = MIRROR_DISABLED;
    int tx, bx;
    int ty = fill->y, by = fill->y + sprite->height - 1;

    gfx_rdp_texture_fill();

    /* Take advantage of native tiling if the sprite is only 1 slice wide */
    if ( slices > 1 )
    {
        /* Small sprites can be drawn using fewer rectangles and tiling */
        rdp_sync( SYNC_PIPE );
        rdp_load_texture( 0, 0, mirror, sprite );
        /* If cut off on the left side, draw clipped version separately */
        tx = scroll_x;
        if ( tx < 0 )
        {
            bx = tx + fill->scroll_w;
            rdp_draw_textured_rectangle( 0, tx, ty, bx, by, mirror );
            tx += fill->scroll_w;
        }
        /* Draw full-tiles for the rest */
        bx = max_w;
        rdp_draw_textured_rectangle( 0, tx, ty, bx, by, mirror );
    }
    else
    {
        /* Manually tile horizontally-sliced repeating fills */
        int slice;
        int repeat_x = scroll_x;
        const int repeat_w = sprite->width / slices;
        while ( repeat_x < max_w )
        {
            for (slice = 0;
                 slice < slices && repeat_x < max_w;
                 slice++, repeat_x += repeat_w)
            {
                tx = repeat_x;
                bx = repeat_x + repeat_w;
                rdp_sync( SYNC_PIPE );
                rdp_load_texture_stride( 0, 0, mirror, sprite, slice );
                rdp_draw_textured_rectangle( 0, tx, ty, bx, by, mirror );
            }
        }
    }
}

void background_draw(const background_t *bg)
{
    /* Color fills */
    background_draw_color( &bg->sky_fill );
    background_draw_color( &bg->cloud_fill );
    background_draw_color( &bg->hill_fill );
    background_draw_color( &bg->ground_fill );

    /* Texture fills */
    background_draw_sprite( bg, &bg->cloud_top );
    background_draw_sprite( bg, &bg->city );
    background_draw_sprite( bg, &bg->hill_top );
    background_draw_sprite( bg, &bg->ground_top );
}
