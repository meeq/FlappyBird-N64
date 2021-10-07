/**
 * FlappyBird-N64 - fps.c
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "fps.h"

#include "gfx.h"

/* FPS definitions */

#define FPS_MAX             ((unsigned int) (60))
#define FPS_FRAME_TICKS     ((unsigned int) ((1000.0 / FPS_MAX) * TICKS_PER_MS))
#define FPS_STAT_TICKS      ((unsigned int) (500 * TICKS_PER_MS))
#define FPS_STATS_PER_SEC   ((double) (FPS_STAT_TICKS / TICKS_PER_MS) / 1000.0)
#define FPS_NUM_HISTORY     ((size_t) 10)
#define FPS_TEXT_LEN        ((size_t) 48)

#define FPS_TEXT_COLOR      graphics_make_color( 0x00, 0x00, 0x00, 0xFF )
#define FPS_CLEAR_COLOR     graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

typedef struct fps_counter_s
{
    bool should_draw;
    ticks_t frame_ticks;
    int total_frames;
    int total_misses;
    ticks_t stat_ticks;
    int frames_per_stat;
    int stat_count;
    double stat_fps[FPS_NUM_HISTORY];
    double average_fps;
} fps_counter_t;

/* FPS implementation */

static fps_counter_t fps = { 0 };

void fps_init(void)
{
    memset(&fps, 0, sizeof fps);
}

void fps_tick(const gamepad_state_t * const gamepad)
{
    /* Toggle drawing flag on C-up */
    if ( gamepad->C_up )
    {
        fps.should_draw = !fps.should_draw;
    }

    /* Increment frame counters */
    fps.total_frames++;
    fps.frames_per_stat++;

    /* Check timing */
    const ticks_t now_ticks = timer_ticks();
    const ticks_t frame_diff = now_ticks - fps.frame_ticks;
    if ( fps.total_frames > 1 && frame_diff > FPS_FRAME_TICKS )
    {
        int frame_period_diff = frame_diff - FPS_FRAME_TICKS;
        fps.total_misses += frame_period_diff / FPS_FRAME_TICKS;
    }
    const ticks_t stat_diff = now_ticks - fps.stat_ticks;
    if ( stat_diff >= FPS_STAT_TICKS )
    {
        double stat_fps = fps.frames_per_stat / FPS_STATS_PER_SEC;
        fps.stat_fps[fps.stat_count % FPS_NUM_HISTORY] = stat_fps;
        if ( fps.stat_count < FPS_NUM_HISTORY )
        {
            fps.stat_count++;
        }

        double total_fps = 0.0;
        for (int i = 0; i < FPS_NUM_HISTORY; i++)
        {
            total_fps += fps.stat_fps[i];
        }
        fps.average_fps = total_fps / fps.stat_count;

        /* Reset the stat counters */
        fps.frames_per_stat = 0;
        int stat_period_diff = stat_diff - FPS_STAT_TICKS;
        fps.stat_ticks = now_ticks - stat_period_diff;
    }
    fps.frame_ticks = now_ticks;
}

void fps_draw(void)
{
    if ( !fps.should_draw ) return;

    gfx_detach_rdp();

    static char fps_text[FPS_TEXT_LEN];
    const ticks_t ticks = timer_ticks();

    graphics_set_color( FPS_TEXT_COLOR, FPS_CLEAR_COLOR );

    const char * const line1_fmt = "FPS: %05.2f, Frame: %u, Miss: %u";
    snprintf( fps_text, FPS_TEXT_LEN, line1_fmt, fps.average_fps, fps.total_frames, fps.total_misses );
    graphics_draw_text( gfx->disp, 10, gfx->height - 33, fps_text );

    const char * const line2_fmt = "Milli: %llu, Tick: %llu";
    snprintf( fps_text, FPS_TEXT_LEN, line2_fmt, ticks / TICKS_PER_MS, ticks );
    graphics_draw_text( gfx->disp, 10, gfx->height - 20, fps_text );
}
