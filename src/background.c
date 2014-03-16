#include "graphics.h"
#include "background.h"


background_t background_setup(u8 day_night_mode)
{
    u32 sky_color, cloud_color, hill_color;
    sprite_t *cloud_sprite, *city_sprite, *hill_sprite;
    if (day_night_mode == DAY_TIME)
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
    background_t result = {
        .day_night_mode = day_night_mode,
        .sky_fill = { .color = sky_color, .y = 0, .h = 141 },
        .cloud_top = { .sprite = cloud_sprite, .y = 119 },
        .cloud_fill = { .color = cloud_color, .y = 141, .h = 22 },
        .city = { .sprite = city_sprite, .y = 148 },
        .hill_top = { .sprite = hill_sprite, .y = 158 },
        .hill_fill = { .color = hill_color, .y = 169, .h = 21 },
        .ground_top = {
            .sprite = read_dfs_sprite( "/gfx/ground.sprite" ),
            .y = 190
        },
        .ground_fill = {
            .color = graphics_make_color( 0xDF, 0xD8, 0x93, 0xFF ),
            .y = 200, .h = 40
        }
    };
    return result;
}

void background_free(background_t bg)
{
    /* Deallocate the sprites */
    free( bg.cloud_top.sprite );
    free( bg.city.sprite );
    free( bg.hill_top.sprite );
    free( bg.ground_top.sprite );
}
