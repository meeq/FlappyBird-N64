#ifndef __FLAPPY_BACKGROUND_H
#define __FLAPPY_BACKGROUND_H

#include "system.h"
#include "graphics.h"

/* Background definitions */

#define BG_SCROLL_RATE 16

#define SKY_COLOR_DAY       graphics_make_color( 0x4E, 0xC0, 0xCA, 0xFF )
#define SKY_COLOR_NIGHT     graphics_make_color( 0x00, 0x87, 0x93, 0xFF )
#define CLOUD_COLOR_DAY     graphics_make_color( 0xE4, 0xFD, 0xD0, 0xFF )
#define CLOUD_COLOR_NIGHT   graphics_make_color( 0x15, 0xA5, 0xB5, 0xFF )
#define HILL_COLOR_DAY      graphics_make_color( 0x52, 0xE0, 0x5D, 0xFF )
#define HILL_COLOR_NIGHT    graphics_make_color( 0x14, 0x96, 0x02, 0xFF )
#define GROUND_COLOR        graphics_make_color( 0xDF, 0xD8, 0x93, 0xFF )

#define SKY_FILL_Y      0
#define SKY_FILL_H      141 * GRAPHICS_SCALE
#define CLOUD_TOP_Y     130 * GRAPHICS_SCALE
#define CLOUD_FILL_Y    141 * GRAPHICS_SCALE
#define CLOUD_FILL_H    32 * GRAPHICS_SCALE
#define CITY_TOP_Y      155 * GRAPHICS_SCALE
#define HILL_TOP_Y      170 * GRAPHICS_SCALE
#define HILL_FILL_Y     179 * GRAPHICS_SCALE
#define HILL_FILL_H     11 * GRAPHICS_SCALE
#define GROUND_TOP_Y    190 * GRAPHICS_SCALE
#define GROUND_FILL_Y   200 * GRAPHICS_SCALE
#define GROUND_FILL_H   40 * GRAPHICS_SCALE

#define SKY_SCROLL_DX       -0.008
#define CITY_SCROLL_DX      -0.04
#define HILL_SCROLL_DX      -0.2
#define GROUND_SCROLL_DX    -1.0

#define DAY_TIME    0
#define NIGHT_TIME  1

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
    double scroll_x;
    u16 scroll_w;
    double scroll_dx;
} bg_fill_sprite_t;

typedef struct
{
    // Setup state
    u8 time_mode;
    u64 scroll_ms;
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

background_t background_setup(u8 time_mode);

void background_free(background_t bg);

void background_tick(background_t *bg);

void draw_bg_fill_color(graphics_t *graphics, bg_fill_color_t fill);
void draw_bg_fill_sprite(graphics_t *graphics, bg_fill_sprite_t fill);

#endif
