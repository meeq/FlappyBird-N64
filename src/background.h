#ifndef __FLAPPY_GAME_H
#define __FLAPPY_GAME_H

#include "system.h"

/* Game definitions */

#define DAY_TIME 0
#define NIGHT_TIME 1

typedef struct
{
    // Setup state
    u8 day_night_mode;
    // Color fills
    x_fill_color_t sky_fill;
    x_fill_color_t cloud_fill;
    x_fill_color_t hill_fill;
    x_fill_color_t ground_fill;
    // Texture fills
    x_fill_sprite_t cloud_top;
    x_fill_sprite_t city;
    x_fill_sprite_t hill_top;
    x_fill_sprite_t ground_top;
} background_t;

/* Game helpers */

background_t background_setup(u8 day_night_mode);

void background_free(background_t bg);

#endif
