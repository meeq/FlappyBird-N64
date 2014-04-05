#include "background.h"

#include "global.h"

background_t background_setup(u8 time_mode)
{
    /* Load the sprite files from the cartridge */
    char *sprite_files[BG_NUM_SPRITES] = {
        "/gfx/bg-cloud-day.sprite", "/gfx/bg-cloud-night.sprite",
        "/gfx/bg-city-day.sprite",  "/gfx/bg-city-night.sprite",
        "/gfx/bg-hill-day.sprite",  "/gfx/bg-hill-night.sprite",
        "/gfx/ground.sprite"
    };
    sprite_t *sprites[BG_NUM_SPRITES];
    for (int i = 0; i < BG_NUM_SPRITES; i++)
    {
        sprites[i] = read_dfs_sprite( sprite_files[i] );
    }
    /* Set up the background fills from top-to-bottom */
    background_t bg = {
        .scroll_ms = 0,
        .sky_fill = {
            .y = SKY_FILL_Y, .h = SKY_FILL_H
        },
        .cloud_top = {
            .y = CLOUD_TOP_Y,
            .scroll_x = 0, .scroll_dx = SKY_SCROLL_DX,
            .scroll_w = sprites[cloud_day_sprite]->width
        },
        .cloud_fill = {
            .y = CLOUD_FILL_Y, .h = CLOUD_FILL_H
        },
        .city = {
            .y = CITY_TOP_Y,
            .scroll_x = 0, .scroll_dx = CITY_SCROLL_DX,
            .scroll_w = sprites[city_day_sprite]->width
        },
        .hill_top = {
            .y = HILL_TOP_Y,
            .scroll_x = 0, .scroll_dx = HILL_SCROLL_DX,
            .scroll_w = sprites[hill_day_sprite]->width
        },
        .hill_fill = {
            .y = HILL_FILL_Y, .h = HILL_FILL_H
        },
        .ground_top = {
            .sprite = ground_sprite,
            .y = GROUND_TOP_Y,
            .scroll_x = 0, .scroll_dx = GROUND_SCROLL_DX,
            .scroll_w = sprites[ground_sprite]->width
        },
        .ground_fill = {
            .color = BG_COLOR_GROUND,
            .y = GROUND_FILL_Y, .h = GROUND_FILL_H
        }
    };
    background_set_time_mode( &bg, time_mode );
    /* Set the sprites on the background struct */
    for (int i = 0; i < BG_NUM_SPRITES; i++)
    {
        bg.sprites[i] = sprites[i];
    }
    return bg;
}

void background_set_time_mode(background_t *bg, u8 time_mode)
{
    bg->time_mode = time_mode;
    if ( time_mode == DAY_TIME )
    {
        bg->sky_fill.color = BG_COLOR_DAY_SKY;
        bg->cloud_fill.color = BG_COLOR_DAY_CLOUD;
        bg->hill_fill.color = BG_COLOR_DAY_HILL;
        bg->cloud_top.sprite = cloud_day_sprite;
        bg->city.sprite = city_day_sprite;
        bg->hill_top.sprite = hill_day_sprite;
    }
    else
    {
        bg->sky_fill.color = BG_COLOR_NIGHT_SKY;
        bg->cloud_fill.color = BG_COLOR_NIGHT_CLOUD;
        bg->hill_fill.color = BG_COLOR_NIGHT_HILL;
        bg->cloud_top.sprite = cloud_night_sprite;
        bg->city.sprite = city_night_sprite;
        bg->hill_top.sprite = hill_night_sprite;
    }
}

void background_free(background_t *bg)
{
    /* Deallocate the sprites */
    for (int i = 0; i < BG_NUM_SPRITES; i++)
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
    while (x > w) x -= w;
    while (x < -w) x += w;
    fill->scroll_x = x;
}

void background_tick(background_t *bg)
{
    const u64 ticks_ms = get_ticks_ms();
    if (ticks_ms - bg->scroll_ms >= BG_SCROLL_RATE)
    {
        bg->scroll_ms = ticks_ms;
        background_tick_scroll( &bg->cloud_top );
        background_tick_scroll( &bg->city );
        background_tick_scroll( &bg->hill_top );
        background_tick_scroll( &bg->ground_top );
    }
}

inline static void background_draw_color(const bg_fill_color_t fill)
{
    graphics_rdp_color_fill( g_graphics );
    rdp_set_primitive_color( fill.color );
    const s16 tx = 0, ty = fill.y;
    const s16 bx = g_graphics->width, by = fill.y + fill.h;
    rdp_draw_filled_rectangle( tx, ty, bx, by );
}

void background_draw_sprite(const background_t bg, const bg_fill_sprite_t fill)
{
    graphics_rdp_texture_fill( g_graphics );
    mirror_t mirror = MIRROR_DISABLED;
    sprite_t *sprite = bg.sprites[fill.sprite];
    const s16 scroll_x = fill.scroll_x;
    const u16 slices = sprite->hslices, max_w = g_graphics->width;
    s16 tx, bx;
    u16 ty = fill.y, by = (fill.y + sprite->height * GRAPHICS_SCALE) - 1;
    if (slices > 1)
    {
        /* Manually tile horizontally-sliced repeating fills */
        int repeat_x = scroll_x;
        const u16 repeat_w = sprite->width / slices;
        u8 slice;
        while (repeat_x < max_w)
        {
            for (slice = 0;
                 slice < slices && repeat_x < max_w;
                 slice++, repeat_x += repeat_w)
            {
                tx = repeat_x * GRAPHICS_SCALE;
                bx = ((repeat_x + repeat_w) * GRAPHICS_SCALE);
                rdp_sync( SYNC_PIPE );
                rdp_load_texture_stride( 0, 0, mirror, sprite, slice );
                rdp_draw_textured_rectangle_scaled( 0,
                    tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
            }
        }
    } else {
        /* Small sprites can be drawn using fewer rectangles and tiling */
        rdp_sync( SYNC_PIPE );
        rdp_load_texture( 0, 0, mirror, sprite );
        /* If cut off on the left side, draw clipped version separately */
        tx = scroll_x;
        if (tx < 0)
        {
            bx = tx + fill.scroll_w;
            rdp_draw_textured_rectangle_scaled( 0,
                tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
            tx += fill.scroll_w;
        }
        /* Draw full-tiles for the rest */
        bx = max_w;
        rdp_draw_textured_rectangle_scaled( 0,
            tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
    }
}

void background_draw(const background_t bg)
{
    /* Color fills */
    background_draw_color( bg.sky_fill );
    background_draw_color( bg.cloud_fill );
    background_draw_color( bg.hill_fill );
    background_draw_color( bg.ground_fill );

    /* Texture fills */
    background_draw_sprite( bg, bg.cloud_top );
    background_draw_sprite( bg, bg.city );
    background_draw_sprite( bg, bg.hill_top );
    background_draw_sprite( bg, bg.ground_top );
}
