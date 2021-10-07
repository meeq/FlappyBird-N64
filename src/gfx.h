/**
 * FlappyBird-N64 - graphics.h
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_GFX_H
#define __FLAPPY_GFX_H

#include "system.h"

/* Graphics definitions */

typedef enum buffer_mode
{
    BUFFERING_DOUBLE = 2,
    BUFFERING_TRIPLE = 3
} buffer_mode_t;

typedef enum rdp_attached
{
    RDP_DETACHED,
    RDP_ATTACHED
} rdp_attached_t;

typedef enum rdp_fill_mode
{
    RDP_FILL_NONE,
    RDP_FILL_COLOR,
    RDP_FILL_TEXTURE
} rdp_fill_mode_t;

typedef struct gfx
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
} gfx_t;

extern gfx_t *gfx;

/* Graphics functions */

void gfx_init(
    resolution_t res, bitdepth_t bit, buffer_mode_t num_buffers,
    gamma_t gamma, antialias_t aa
);

void gfx_close(void);

void gfx_display_lock(void);
void gfx_display_flip(void);

void gfx_attach_rdp(void);
void gfx_detach_rdp(void);

void gfx_rdp_color_fill(void);
void gfx_rdp_texture_fill(void);

sprite_t *read_dfs_sprite(char *file);

#endif
