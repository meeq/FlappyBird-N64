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

/* Font IDs for RDPQ text rendering */
#define FONT_DEBUG 1

/* Graphics definitions */

#define GFX_BASE_WIDTH  320
#define GFX_BASE_HEIGHT 240

typedef struct gfx_s
{
    // Setup state
    int width;
    int height;
    float scale;    // 1.0 for 320x240, 2.0 for 640x480
    bool highres;   // true if in high-res mode
    // Drawing state
    surface_t *disp;
} gfx_t;

extern gfx_t *gfx;

/* Scale a value by the current graphics scale factor */
#define GFX_SCALE(v) ((int)((v) * gfx->scale))
#define GFX_SCALEF(v) ((v) * gfx->scale)

/* Graphics functions */

void gfx_init(void);

void gfx_display_lock(void);

void gfx_attach_rdp(void);

void gfx_set_highres(bool enable);

bool gfx_get_highres(void);

sprite_t *read_dfs_sprite(const char *file);

#endif
