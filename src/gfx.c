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

void gfx_init(
    resolution_t res, bitdepth_t depth, buffer_mode_t num_buffers,
    gamma_t gamma, antialias_t aa)
{
    /* Set up the display and RDP subsystems */
    display_init(res, depth, num_buffers, gamma, aa);
    rdp_init();
    gfx = malloc(sizeof(gfx_t));
    /* Setup state */
    gfx->res = res;
    gfx->width = res.width;
    gfx->height = res.height;
    gfx->color_depth = depth;
    gfx->num_buffers = num_buffers;
    gfx->gamma = gamma;
    gfx->antialias = aa;
    /* Drawing state */
    gfx->disp = 0;
    /* RDP state */
    gfx->rdp_attached = RDP_DETACHED;
    gfx->rdp_fill_mode = RDP_FILL_NONE;
}

void gfx_close(void)
{
    rdp_close();
    display_close();
    free(gfx);
    gfx = NULL;
}

void gfx_display_lock(void)
{
    /* Grab a render buffer */
    static display_context_t disp = 0;
    while (!(disp = display_lock())) { /* Spinlock! */ }
    gfx->disp = disp;
    /* Reset RDP state */
    gfx->rdp_attached = RDP_DETACHED;
    gfx->rdp_fill_mode = RDP_FILL_NONE;
}

void gfx_display_flip(void)
{
    if (gfx->disp)
    {
        /* Detach the RDP and sync before flipping the display buffer */
        if (gfx->rdp_attached == RDP_ATTACHED)
        {
            gfx_detach_rdp();
        }
        /* Force backbuffer flip and reset the display handle */
        display_show(gfx->disp);
        gfx->disp = 0;
    }
}

void gfx_attach_rdp(void)
{
    if (gfx->rdp_attached == RDP_DETACHED && gfx->disp)
    {
        /* Ensure the RDP is ready for new commands */
        rdp_sync(SYNC_PIPE);

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Attach RDP to display */
        rdp_attach(gfx->disp);
        gfx->rdp_attached = RDP_ATTACHED;
    }
}

void gfx_detach_rdp(void)
{
    if (gfx->rdp_attached == RDP_ATTACHED)
    {
        /* Inform the RDP drawing is finished; flush pending operations */
        rdp_detach();
        gfx->rdp_attached = RDP_DETACHED;
        gfx->rdp_fill_mode = RDP_FILL_NONE;
    }
}

void gfx_rdp_color_fill(void)
{
    gfx_attach_rdp();
    /* Setup the RDP for color fills if it isn't already */
    if (gfx->rdp_fill_mode != RDP_FILL_COLOR)
    {
        /* Enable solid colors instead of texture fills */
        rdp_enable_primitive_fill();
        gfx->rdp_fill_mode = RDP_FILL_COLOR;
    }
}

void gfx_rdp_texture_fill(void)
{
    gfx_attach_rdp();
    /* Setup the RDP for textured fills if it isn't already */
    if (gfx->rdp_fill_mode != RDP_FILL_TEXTURE)
    {
        /* Enable textures instead of solid color fill */
        rdp_enable_texture_copy();
        gfx->rdp_fill_mode = RDP_FILL_TEXTURE;
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
