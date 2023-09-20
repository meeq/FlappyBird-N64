/**
 * FlappyBird-N64 - graphics.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_GFX_H
#define __FLAPPY_GFX_H

#include "system.h"

/* Graphics definitions */

typedef struct gfx_s
{
    // Setup state
    int width;
    int height;
    // Drawing state
    surface_t *disp;
} gfx_t;

extern gfx_t *gfx;

/* Graphics functions */

void gfx_init(void);

void gfx_display_lock(void);

void gfx_attach_rdp(void);
void gfx_detach_rdp(void);

void gfx_rdp_color_fill(color_t color);
void gfx_rdp_texture_fill(bool transparency);

sprite_t *read_dfs_sprite(const char *file);

#endif
