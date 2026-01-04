/**
 * FlappyBird-N64 - gfx.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include "gfx.h"

gfx_t *gfx;

/* Font IDs for RDPQ text rendering */
#define FONT_DEBUG 1

void gfx_init(void)
{
    /* Set up the display and RDP subsystems */
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    rdpq_init();
    /* Setup debug font for text rendering */
    rdpq_font_t *debug_font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(FONT_DEBUG, debug_font);
    /* Setup state */
    gfx = malloc(sizeof(gfx_t));
    gfx->width = display_get_width();
    gfx->height = display_get_height();
    gfx->disp = NULL;
}

void gfx_display_lock(void)
{
    /* Grab a render buffer */
    surface_t *disp = display_get();
    rdpq_attach_clear(disp, NULL);
    gfx->disp = disp;
}

void gfx_attach_rdp(void)
{
    if (!rdpq_is_attached() && gfx->disp)
    {
        /* Attach RDP to display - RDPQ handles sync automatically */
        rdpq_attach(gfx->disp, NULL);
    }
}

sprite_t *read_dfs_sprite(const char *const file)
{
    int fp = dfs_open(file);
    sprite_t *sprite = malloc(dfs_size(fp));
    dfs_read(sprite, 1, dfs_size(fp), fp);
    dfs_close(fp);
    return sprite;
}
