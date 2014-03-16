#ifndef __FLAPPY_GAME_H
#define __FLAPPY_GAME_H

#include "system.h"
#include "graphics.h"

/* Background definitions */

#define DAY_TIME 0
#define NIGHT_TIME 1

typedef struct
{
    u32 color;
    s16 y;
    s16 h;
} bg_fill_color_t;

typedef struct
{
    sprite_t *sprite;
    s16 y;
} bg_fill_sprite_t;

typedef struct
{
    // Setup state
    u8 day_night_mode;
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

/* Background helpers */

background_t background_setup(u8 day_night_mode);

void background_free(background_t bg);

void draw_bg_fill_color(graphics_t *graphics, bg_fill_color_t fill);
void draw_bg_fill_sprite(graphics_t *graphics, bg_fill_sprite_t fill);

#endif
