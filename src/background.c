#include "background.h"


background_t background_setup(u8 time_mode)
{
    u32 sky_color, cloud_color, hill_color;
    sprite_t *cloud_sprite, *city_sprite, *hill_sprite, *ground_sprite;
    if (time_mode == DAY_TIME)
    {
        sky_color = SKY_COLOR_DAY;
        cloud_color = CLOUD_COLOR_DAY;
        hill_color = HILL_COLOR_DAY;
        cloud_sprite = read_dfs_sprite( "/gfx/bg-cloud-day.sprite" );
        city_sprite = read_dfs_sprite( "/gfx/bg-city-day.sprite" );
        hill_sprite = read_dfs_sprite( "/gfx/bg-hill-day.sprite" );
    } else {
        sky_color = SKY_COLOR_NIGHT;
        cloud_color = CLOUD_COLOR_NIGHT;
        hill_color = HILL_COLOR_NIGHT;
        cloud_sprite = read_dfs_sprite( "/gfx/bg-cloud-night.sprite" );
        city_sprite = read_dfs_sprite( "/gfx/bg-city-night.sprite" );
        hill_sprite = read_dfs_sprite( "/gfx/bg-hill-night.sprite" );
    }
    ground_sprite = read_dfs_sprite( "/gfx/ground.sprite" );
    /* Set up the background fills from top-to-bottom */
    background_t background = {
        .time_mode = time_mode,
        .scroll_ms = 0,
        .sky_fill = {
            .color = sky_color, .y = SKY_FILL_Y, .h = SKY_FILL_H
        },
        .cloud_top = {
            .sprite = cloud_sprite, .y = CLOUD_TOP_Y,
            .scroll_x = 0, .scroll_dx = SKY_SCROLL_DX,
            .scroll_w = (cloud_sprite->width / cloud_sprite->hslices)
        },
        .cloud_fill = {
            .color = cloud_color, .y = CLOUD_FILL_Y, .h = CLOUD_FILL_H
        },
        .city = {
            .sprite = city_sprite, .y = CITY_TOP_Y,
            .scroll_x = 0, .scroll_dx = CITY_SCROLL_DX,
            .scroll_w = (city_sprite->width / city_sprite->hslices)
        },
        .hill_top = {
            .sprite = hill_sprite, .y = HILL_TOP_Y,
            .scroll_x = 0, .scroll_dx = HILL_SCROLL_DX,
            .scroll_w = (hill_sprite->width / hill_sprite->hslices)
        },
        .hill_fill = {
            .color = hill_color, .y = HILL_FILL_Y, .h = HILL_FILL_H
        },
        .ground_top = {
            .sprite = ground_sprite, .y = GROUND_TOP_Y,
            .scroll_x = 0, .scroll_dx = GROUND_SCROLL_DX,
            .scroll_w = (ground_sprite->width / ground_sprite->hslices)
        },
        .ground_fill = {
            .color = GROUND_COLOR, .y = GROUND_FILL_Y, .h = GROUND_FILL_H
        }
    };
    return background;
}

void background_free(background_t bg)
{
    /* Deallocate the sprites */
    free( bg.cloud_top.sprite );
    free( bg.city.sprite );
    free( bg.hill_top.sprite );
    free( bg.ground_top.sprite );
}

inline void background_tick_scroll(bg_fill_sprite_t *fill)
{
    double x = fill->scroll_x;
    u16 w = fill->scroll_w;
    x += fill->scroll_dx;
    while (x > w) x -= w;
    while (x < -w) x += w;
    fill->scroll_x = x;
}

void background_tick(background_t *bg)
{
    u64 ticks_ms = get_ticks_ms();
    if (ticks_ms - bg->scroll_ms >= BACKGROUND_SCROLL_RATE)
    {
        bg->scroll_ms = ticks_ms;
        background_tick_scroll( &bg->cloud_top );
        background_tick_scroll( &bg->city );
        background_tick_scroll( &bg->hill_top );
        background_tick_scroll( &bg->ground_top );
    }
}

void background_draw_color(graphics_t *graphics, bg_fill_color_t fill)
{
    graphics_rdp_color_fill( graphics );
    rdp_set_primitive_color( fill.color );
    rdp_draw_filled_rectangle( 0, fill.y, graphics->width, fill.y + fill.h );
}

void background_draw_sprite(graphics_t *graphics, bg_fill_sprite_t fill)
{
    graphics_rdp_texture_fill( graphics );
    mirror_t mirror = MIRROR_DISABLED;
    sprite_t *sprite = fill.sprite;
    s16 scroll_x = fill.scroll_x, tx, bx;
    u16 ty = fill.y, by = (fill.y + sprite->height * GRAPHICS_SCALE) - 1;
    int slices = sprite->hslices, max_w = graphics->width;
    if (slices > 1)
    {
        /* Manually tile horizontally-sliced repeating fills */
        for (int repeat_x = scroll_x,
                 repeat_w = sprite->width / slices,
                 slice;
             repeat_x < max_w;)
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
