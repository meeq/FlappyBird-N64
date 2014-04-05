#ifndef __FLAPPY_GRAPHICS_H
#define __FLAPPY_GRAPHICS_H

#include "system.h"

/* Graphics definitions */

#define GRAPHICS_SCALE 1

typedef enum graphics_buffering_mode
{
    BUFFERING_DOUBLE = 2,
    BUFFERING_TRIPLE = 3
} buffer_mode_t;

typedef enum graphics_rdp_attached_state
{
    RDP_DETACHED,
    RDP_ATTACHED
} rdp_attached_t;

typedef enum graphics_rdp_fill_state
{
    RDP_FILL_NONE,
    RDP_FILL_COLOR,
    RDP_FILL_TEXTURE
} rdp_fill_mode_t;

typedef struct
{
    // Setup state
    resolution_t res;
    u16 width;
    u16 height;
    bitdepth_t color_depth;
    buffer_mode_t num_buffers;
    gamma_t gamma;
    antialias_t antialias;
    // Drawing state
    u32 disp;
    // RDP state
    rdp_attached_t rdp_attached;
    rdp_fill_mode_t rdp_fill_mode;
} graphics_t;

/* Graphics helpers */

graphics_t *graphics_setup(resolution_t res, bitdepth_t bit,
                           buffer_mode_t num_buffers, gamma_t gamma,
                           antialias_t aa);

void graphics_free(graphics_t *graphics);

void graphics_display_lock(graphics_t *graphics);
void graphics_display_flip(graphics_t *graphics);

void graphics_attach_rdp(graphics_t *graphics);
void graphics_detach_rdp(graphics_t *graphics);

void graphics_rdp_color_fill(graphics_t *graphics);
void graphics_rdp_texture_fill(graphics_t *graphics);

sprite_t *read_dfs_sprite(char *file);

#endif
