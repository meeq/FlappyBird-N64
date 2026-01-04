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
    /* Setup state */
    gfx = malloc(sizeof(gfx_t));
    gfx->scale = 1.0f;
    gfx->highres = false;
    gfx->disp = NULL;
    /* Set up the display and RDP subsystems */
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    rdpq_init();
    /* Load custom fonts for text rendering (1x and 2x for high-res) */
    rdpq_font_t *font_1x = rdpq_font_load("rom:/fonts/at01-1x.font64");
    rdpq_font_t *font_2x = rdpq_font_load("rom:/fonts/at01-2x.font64");
    rdpq_text_register_font(FONT_AT01, font_1x);
    rdpq_text_register_font(FONT_AT01_2X, font_2x);
    /* Cache display dimensions */
    gfx->width = display_get_width();
    gfx->height = display_get_height();
}

void gfx_set_highres(bool enable)
{
    if (gfx->highres == enable) return;

    /* Wait for any pending RDP operations to complete */
    rspq_wait();

    /* Close the current display */
    display_close();

    /* Initialize with new resolution */
    if (enable)
    {
        /* High-res: 640x480 interlaced with better filtering */
        display_init(RESOLUTION_640x480, DEPTH_16_BPP, 3, GAMMA_NONE,
                     FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
        gfx->scale = 2.0f;
    }
    else
    {
        /* Low-res: 320x240 progressive */
        display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE,
                     FILTERS_RESAMPLE);
        gfx->scale = 1.0f;
    }

    gfx->highres = enable;
    gfx->width = display_get_width();
    gfx->height = display_get_height();
}

bool gfx_get_highres(void)
{
    return gfx->highres;
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
