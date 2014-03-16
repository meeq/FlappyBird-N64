#ifndef __FLAPPY_GRAPHICS_H
#define __FLAPPY_GRAPHICS_H

#include "system.h"

/* Graphics definitions */

#define RDP_DETACHED 0
#define RDP_ATTACHED 1

#define RDP_FILL_NONE 0
#define RDP_FILL_COLOR 1
#define RDP_FILL_TEXTURE 2

typedef struct
{
    // Setup state
    resolution_t res;
    u16 width;
    u16 height;
    bitdepth_t color_depth;
    u32 num_buffers;
    gamma_t gamma;
    antialias_t antialias;
    // Drawing state
    u32 disp;
    // RDP state
    u8 rdp_attached;
    u8 rdp_fill_mode;
} graphics_t;

typedef struct
{
    u32 color;
    s16 y;
    s16 h;
} x_fill_color_t;

typedef struct
{
    sprite_t *sprite;
    s16 y;
} x_fill_sprite_t;

/* Graphics helpers */

graphics_t *graphics_setup(resolution_t res, bitdepth_t bit,
                           u32 num_buffers, gamma_t gamma,
                           antialias_t aa);

void free_graphics(graphics_t *graphics);

sprite_t *read_dfs_sprite(char *file);

void draw_x_fill_color(graphics_t *graphics, x_fill_color_t fill);
void draw_x_fill_sprite(graphics_t *graphics, x_fill_sprite_t fill);

#endif
