#include "background.h"


background_t background_setup(u8 time_mode)
{
    u32 sky_color, cloud_color, hill_color;
    sprite_t *cloud_sprite, *city_sprite, *hill_sprite;
    if (time_mode == DAY_TIME)
    {
        sky_color = graphics_make_color( 0x4E, 0xC0, 0xCA, 0xFF );
        cloud_color = graphics_make_color( 0xE9, 0xFC, 0xD9, 0xFF );
        hill_color = graphics_make_color( 0x5E, 0xE2, 0x70, 0xFF );
        cloud_sprite = read_dfs_sprite( "/gfx/bg-cloud-day.sprite" );
        city_sprite = read_dfs_sprite( "/gfx/bg-city-day.sprite" );
        hill_sprite = read_dfs_sprite( "/gfx/bg-hill-day.sprite" );
    } else {
        sky_color = graphics_make_color( 0x00, 0x87, 0x93, 0xFF );
        cloud_color = graphics_make_color( 0x00, 0xB3, 0xC2, 0xFF );
        hill_color = graphics_make_color( 0x00, 0xA3, 0x00, 0xFF );
        cloud_sprite = read_dfs_sprite( "/gfx/bg-cloud-night.sprite" );
        city_sprite = read_dfs_sprite( "/gfx/bg-city-night.sprite" );
        hill_sprite = read_dfs_sprite( "/gfx/bg-hill-night.sprite" );
    }
    /* Set up the background fills from top-to-bottom */
    /* These are all hard-coded for 320x240 resolution... */
    background_t background = {
        .time_mode = time_mode,
        .sky_fill = {
            .color = sky_color, .y = SKY_FILL_Y, .h = SKY_FILL_H
        },
        .cloud_top = {
            .sprite = cloud_sprite, .y = CLOUD_TOP_Y
        },
        .cloud_fill = {
            .color = cloud_color, .y = CLOUD_FILL_Y, .h = CLOUD_FILL_H
        },
        .city = {
            .sprite = city_sprite, .y = CITY_TOP_Y
        },
        .hill_top = {
            .sprite = hill_sprite, .y = HILL_TOP_Y
        },
        .hill_fill = {
            .color = hill_color, .y = HILL_FILL_Y, .h = HILL_FILL_H
        },
        .ground_top = {
            .sprite = read_dfs_sprite( "/gfx/ground.sprite" ),
            .y = GROUND_TOP_Y
        },
        .ground_fill = {
            .color = graphics_make_color( 0xDF, 0xD8, 0x93, 0xFF ),
            .y = GROUND_FILL_Y, .h = GROUND_FILL_H
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

void draw_bg_fill_color(graphics_t *graphics, bg_fill_color_t fill)
{
    graphics_rdp_color_fill( graphics );
    rdp_set_primitive_color( fill.color );
    rdp_draw_filled_rectangle( 0, fill.y, graphics->width, fill.y + fill.h );
}

void draw_bg_fill_sprite(graphics_t *graphics, bg_fill_sprite_t fill)
{
    graphics_rdp_texture_fill( graphics );
    mirror_t mirror = MIRROR_DISABLED;
    sprite_t *sprite = fill.sprite;
    u16 ty = fill.y, by = (fill.y + sprite->height * GRAPHICS_SCALE) - 1;
    int slices = sprite->hslices, max_w = graphics->width;
    if (slices > 1)
    {
        u16 tx, bx;
        /* Manually tile horizontally-sliced repeating fills */
        for (int repeat_x = 0,
                 repeat_w = sprite->width / slices,
                 slice;
             repeat_x < max_w;)
        {
            for (slice = 0;
                 slice < slices && repeat_x < max_w;
                 slice++, repeat_x += repeat_w)
            {
                tx = repeat_x * GRAPHICS_SCALE;
                bx = ((repeat_x + repeat_w) * GRAPHICS_SCALE) - 1;
                rdp_sync( SYNC_PIPE );
                rdp_load_texture_stride( 0, 0, mirror, sprite, slice );
                rdp_draw_textured_rectangle_scaled( 0,
                    tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
            }
        }
    } else {
        /* Small tiles can be drawn in a single rectangle */
        rdp_sync( SYNC_PIPE );
        rdp_load_texture( 0, 0, mirror, sprite );
        u16 tx = 0, bx = max_w;
        rdp_draw_textured_rectangle_scaled( 0,
            tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
    }
}
