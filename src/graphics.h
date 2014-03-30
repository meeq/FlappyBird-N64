#ifndef __FLAPPY_GRAPHICS_H
#define __FLAPPY_GRAPHICS_H

#include "system.h"

/* Graphics definitions */

#define GRAPHICS_SCALE 1

#define BUFFERING_DOUBLE 2
#define BUFFERING_TRIPLE 3

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

/* Graphics helpers */

graphics_t *graphics_setup(resolution_t res, bitdepth_t bit,
                           u32 num_buffers, gamma_t gamma,
                           antialias_t aa);
void free_graphics(graphics_t *graphics);

void graphics_display_lock(graphics_t *graphics);
void graphics_display_flip(graphics_t *graphics);

void graphics_attach_rdp(graphics_t *graphics);
void graphics_detach_rdp(graphics_t *graphics);

void graphics_rdp_color_fill(graphics_t *graphics);
void graphics_rdp_texture_fill(graphics_t *graphics);

sprite_t *read_dfs_sprite(char *file);

#endif
