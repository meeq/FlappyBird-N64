/**
 * FlappyBird-N64 - background.c
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <stdlib.h>

#include "background.h"

#include "global.h"

background_t background_setup(bg_time_mode_t time_mode)
{
    /* Load the sprite files from the cartridge */
    char *sprite_files[BG_NUM_SPRITES] = {
        "/gfx/bg-cloud-day.sprite", "/gfx/bg-cloud-night.sprite",
        "/gfx/bg-city-day.sprite",  "/gfx/bg-city-night.sprite",
        "/gfx/bg-hill-day.sprite",  "/gfx/bg-hill-night.sprite",
        "/gfx/ground.sprite"
    };
    sprite_t *sprites[BG_NUM_SPRITES];
    for (u8 i = 0; i < BG_NUM_SPRITES; i++)
    {
        sprites[i] = read_dfs_sprite( sprite_files[i] );
    }
    /* Set up the background fills from top-to-bottom */
    background_t bg = {
        .scroll_ms = 0,
        .sky_fill = {
            .y = BG_SKY_FILL_Y, .h = BG_SKY_FILL_H
        },
        .cloud_top = {
            .y = BG_CLOUD_TOP_Y,
            .scroll_x = 0, .scroll_dx = BG_SKY_SCROLL_DX,
            .scroll_w = sprites[BG_CLOUD_DAY_SPRITE]->width
        },
        .cloud_fill = {
            .y = BG_CLOUD_FILL_Y, .h = BG_CLOUD_FILL_H
        },
        .city = {
            .y = BG_CITY_TOP_Y,
            .scroll_x = 0, .scroll_dx = BG_CITY_SCROLL_DX,
            .scroll_w = sprites[BG_CITY_DAY_SPRITE]->width
        },
        .hill_top = {
            .y = BG_HILL_TOP_Y,
            .scroll_x = 0, .scroll_dx = BG_HILL_SCROLL_DX,
            .scroll_w = sprites[BG_HILL_DAY_SPRITE]->width
        },
        .hill_fill = {
            .y = BG_HILL_FILL_Y, .h = BG_HILL_FILL_H
        },
        .ground_top = {
            .sprite = BG_GROUND_SPRITE,
            .y = BG_GROUND_TOP_Y,
            .scroll_x = 0, .scroll_dx = BG_GROUND_SCROLL_DX,
            .scroll_w = sprites[BG_GROUND_SPRITE]->width
        },
        .ground_fill = {
            .color = BG_COLOR_GROUND,
            .y = BG_GROUND_FILL_Y, .h = BG_GROUND_FILL_H
        }
    };
    background_set_time_mode( &bg, time_mode );
    /* Set the sprites on the background struct */
    for (u8 i = 0; i < BG_NUM_SPRITES; i++)
    {
        bg.sprites[i] = sprites[i];
    }
    return bg;
}

void background_set_time_mode(background_t *bg, bg_time_mode_t time_mode)
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

void background_randomize_time_mode(background_t *bg)
{
    background_set_time_mode( bg, background_random_time_mode() );
}

void background_free(background_t *bg)
{
    /* Deallocate the sprites */
    for (u8 i = 0; i < BG_NUM_SPRITES; i++)
    {
        free( bg->sprites[i] );
        bg->sprites[i] = NULL;
    }
}

inline void background_tick_scroll(bg_fill_sprite_t *fill)
{
    double x = fill->scroll_x;
    const u16 w = fill->scroll_w;
    x += fill->scroll_dx;
    while ( x > w ) x -= w;
    while ( x < -w ) x += w;
    fill->scroll_x = x;
}

void background_tick(background_t *bg, const gamepad_state_t *gamepad)
{
     /* Switch between day and night */
    if( gamepad->L )
    {
        background_set_time_mode( bg, !bg->time_mode );
    }
    /* Scroll the background */
    const u32 ticks_ms = get_total_ms();
    if ( ticks_ms - bg->scroll_ms >= BG_SCROLL_RATE )
    {
        bg->scroll_ms = ticks_ms;
        background_tick_scroll( &bg->cloud_top );
        background_tick_scroll( &bg->city );
        background_tick_scroll( &bg->hill_top );
        background_tick_scroll( &bg->ground_top );
    }
}

inline static void background_draw_color(const bg_fill_color_t *fill)
{
    graphics_rdp_color_fill( g_graphics );
    rdp_set_primitive_color( fill->color );
    const s16 tx = 0, ty = fill->y;
    const s16 bx = g_graphics->width, by = fill->y + fill->h;
    rdp_draw_filled_rectangle( tx, ty, bx, by );
}

void background_draw_sprite(const background_t *bg,
                            const bg_fill_sprite_t *fill)
{
    sprite_t *sprite = bg->sprites[fill->sprite];
    if ( sprite == NULL ) return;

    const s16 scroll_x = fill->scroll_x;
    const u16 slices = sprite->hslices, max_w = g_graphics->width;
    const mirror_t mirror = MIRROR_DISABLED;
    s16 tx, bx;
    u16 ty = fill->y, by = fill->y + sprite->height - 1;

    graphics_rdp_texture_fill( g_graphics );

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
        u8 slice;
        s16 repeat_x = scroll_x;
        const u16 repeat_w = sprite->width / slices;
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
