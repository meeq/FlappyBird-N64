/**
 * FlappyBird-N64 - fps.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include "fps.h"

#include "gfx.h"

/* FPS definitions */

#define FPS_MAX             ((unsigned int) (60))
#define FPS_FRAME_TICKS     ((unsigned int) ((1000.0 / FPS_MAX) * TICKS_PER_MS))

typedef struct fps_counter_s
{
    bool should_draw;
    ticks_t frame_ticks;
    int total_frames;
    int total_misses;
} fps_counter_t;

/* FPS implementation */

static fps_counter_t fps = {0};

void fps_init(void)
{
    memset(&fps, 0, sizeof fps);
    color_t color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF);
    /* Setup font style for FPS text (both 1x and 2x fonts) */
    rdpq_font_t *font_1x = (rdpq_font_t *)rdpq_text_get_font(FONT_AT01);
    rdpq_font_t *font_2x = (rdpq_font_t *)rdpq_text_get_font(FONT_AT01_2X);
    if (font_1x)
    {
        rdpq_font_style(font_1x, 0, &(rdpq_fontstyle_t){ .color = color });
    }
    if (font_2x)
    {
        rdpq_font_style(font_2x, 0, &(rdpq_fontstyle_t){ .color = color });
    }
}

void fps_tick(const joypad_buttons_t *buttons)
{
    /* Toggle drawing flag on C-up */
    if (buttons->c_up)
    {
        fps.should_draw = !fps.should_draw;
    }

    fps.total_frames++;

    /* Track missed frames */
    const ticks_t now_ticks = timer_ticks();
    const ticks_t frame_diff = now_ticks - fps.frame_ticks;
    if (fps.total_frames > 1 && frame_diff > FPS_FRAME_TICKS)
    {
        int frame_period_diff = frame_diff - FPS_FRAME_TICKS;
        fps.total_misses += frame_period_diff / FPS_FRAME_TICKS;
    }
    fps.frame_ticks = now_ticks;
}

void fps_set_visible(bool visible)
{
    fps.should_draw = visible;
}

bool fps_get_visible(void)
{
    return fps.should_draw;
}

void fps_draw(void)
{
    if (!fps.should_draw) return;

    const ticks_t ticks = timer_ticks();
    const int font_id = gfx->highres ? FONT_AT01_2X : FONT_AT01;
    const int line_height = gfx->highres ? 26 : 13;

    rdpq_text_printf(NULL, font_id, 10, gfx->height - (line_height * 2 + 7),
        "FPS: %05.2f, Frame: %u, Miss: %u",
        display_get_fps(), fps.total_frames, fps.total_misses);

    rdpq_text_printf(NULL, font_id, 10, gfx->height - (line_height + 7),
        "Milli: %llu, Tick: %llu",
        ticks / TICKS_PER_MS, ticks);
}
