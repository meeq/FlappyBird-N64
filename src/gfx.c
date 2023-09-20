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

void gfx_init(void)
{
    /* Set up the display and RDP subsystems */
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    rdp_init(); // TODO Replace with rdpq_init()
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
        /* Ensure the RDP is ready for new commands */
        rdp_sync(SYNC_PIPE);

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Attach RDP to display */
        rdp_attach(gfx->disp);
    }
}

void gfx_detach_rdp(void)
{
    if (rdpq_is_attached())
    {
        /* Inform the RDP drawing is finished; flush pending operations */
        rdpq_detach_wait();
    }
}

void gfx_rdp_color_fill(color_t color)
{
    gfx_attach_rdp();
    rdpq_set_mode_fill(color);
}

void gfx_rdp_texture_fill(bool transparency)
{
    gfx_attach_rdp();
    rdpq_set_mode_copy(transparency);
}

sprite_t *read_dfs_sprite(const char *const file)
{
    int fp = dfs_open(file);
    sprite_t *sprite = malloc(dfs_size(fp));
    dfs_read(sprite, 1, dfs_size(fp), fp);
    dfs_close(fp);
    return sprite;
}
