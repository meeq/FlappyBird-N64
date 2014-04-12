#ifndef __FLAPPY_BACKGROUND_H
#define __FLAPPY_BACKGROUND_H

#include "system.h"
#include "graphics.h"

/* Background definitions */

#define BG_SCROLL_RATE          16

#define BG_COLOR_DAY_SKY        graphics_make_color( 0x4E, 0xC0, 0xCA, 0xFF )
#define BG_COLOR_NIGHT_SKY      graphics_make_color( 0x00, 0x87, 0x93, 0xFF )
#define BG_COLOR_DAY_CLOUD      graphics_make_color( 0xE4, 0xFD, 0xD0, 0xFF )
#define BG_COLOR_NIGHT_CLOUD    graphics_make_color( 0x15, 0xA5, 0xB5, 0xFF )
#define BG_COLOR_DAY_HILL       graphics_make_color( 0x52, 0xE0, 0x5D, 0xFF )
#define BG_COLOR_NIGHT_HILL     graphics_make_color( 0x14, 0x96, 0x02, 0xFF )
#define BG_COLOR_GROUND         graphics_make_color( 0xDF, 0xD8, 0x93, 0xFF )

#define BG_SKY_FILL_Y           0
#define BG_SKY_FILL_H           141
#define BG_CLOUD_TOP_Y          130
#define BG_CLOUD_FILL_Y         141
#define BG_CLOUD_FILL_H         32
#define BG_CITY_TOP_Y           155
#define BG_HILL_TOP_Y           170
#define BG_HILL_FILL_Y          179
#define BG_HILL_FILL_H          11
#define BG_GROUND_TOP_Y         190
#define BG_GROUND_FILL_Y        200
#define BG_GROUND_FILL_H        40

#define BG_SKY_SCROLL_DX        -0.008
#define BG_CITY_SCROLL_DX       -0.04
#define BG_HILL_SCROLL_DX       -0.2
#define BG_GROUND_SCROLL_DX     -1.0

#define BG_NUM_SPRITES          7

typedef enum bg_sprite_index
{
    BG_CLOUD_DAY_SPRITE, BG_CLOUD_NIGHT_SPRITE,
    BG_CITY_DAY_SPRITE,  BG_CITY_NIGHT_SPRITE,
    BG_HILL_DAY_SPRITE,  BG_HILL_NIGHT_SPRITE,
    BG_GROUND_SPRITE
} bg_sprite_index_t;

typedef enum bg_time_mode
{
    BG_DAY_TIME,
    BG_NIGHT_TIME,
    BG_NUM_TIME_MODES
} bg_time_mode_t;

typedef struct
{
    u32 color;
    s16 y;
    s16 h;
} bg_fill_color_t;

typedef struct
{
    bg_sprite_index_t sprite;
    s16 y;
    double scroll_x;
    u16 scroll_w;
    double scroll_dx;
} bg_fill_sprite_t;

typedef struct
{
    sprite_t *sprites[BG_NUM_SPRITES];
    // Setup state
    bg_time_mode_t time_mode;
    u32 scroll_ms;
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

/* Background functions */

background_t background_setup(bg_time_mode_t time_mode);

void background_free(background_t *bg);

void background_set_time_mode(background_t *bg, bg_time_mode_t time_mode);

void background_randomize_time_mode(background_t *bg);

void background_tick(background_t *bg, const gamepad_state_t gamepad);

void background_draw(const background_t bg);

#endif
