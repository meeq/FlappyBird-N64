/**
 * FlappyBird-N64 - fps.c
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "fps.h"

#include "global.h"

fps_counter_t fps_setup(void)
{
    fps_counter_t fps = {
        .should_draw = false,
        .frame_ms = 0,
        .total_frames = 0,
        .total_misses = 0,
        .stat_ms = 0,
        .frames_per_stat = 0,
        .stat_count = 0,
        .average_fps = 0.0
    };
    for (u8 i = 0; i < FPS_NUM_HISTORY; i++)
    {
        fps.stat_fps[i] = 0.0;
    }
    return fps;
}

void fps_tick(fps_counter_t *fps, const gamepad_state_t *gamepad)
{
    /* Toggle drawing flag on C-up */
    if ( gamepad->C_up )
    {
        fps->should_draw = !fps->should_draw;
    }

    /* Increment frame counters */
    fps->total_frames++;
    fps->frames_per_stat++;

    /* Check timing */
    const u32 ticks_ms = get_total_ms();
    const s32 frame_diff_ms = ticks_ms - fps->frame_ms;
    if ( fps->total_frames > 1 && frame_diff_ms > FPS_FRAME_PERIOD )
    {
        s32 frame_period_diff = frame_diff_ms - FPS_FRAME_PERIOD;
        fps->total_misses += frame_period_diff / FPS_FRAME_PERIOD;
    }
    const s32 stat_diff_ms = ticks_ms - fps->stat_ms;
    if ( stat_diff_ms >= FPS_STAT_PERIOD )
    {
        double stat_fps = fps->frames_per_stat / FPS_STATS_PER_SEC;
        fps->stat_fps[fps->stat_count % FPS_NUM_HISTORY] = stat_fps;
        if ( fps->stat_count < FPS_NUM_HISTORY )
        {
            fps->stat_count++;
        }

        double total_fps = 0.0;
        for (int i = 0; i < FPS_NUM_HISTORY; i++)
        {
            total_fps += fps->stat_fps[i];
        }
        fps->average_fps = total_fps / fps->stat_count;

        /* Reset the stat counters */
        fps->frames_per_stat = 0;
        u16 stat_period_diff = stat_diff_ms - FPS_STAT_PERIOD;
        fps->stat_ms = ticks_ms - stat_period_diff;
    }
    fps->frame_ms = ticks_ms;
}

void fps_draw(const fps_counter_t *fps)
{
    if ( !fps->should_draw ) return;

    gfx_detach_rdp();

    char fps_text[48];
    graphics_set_color( FPS_TEXT_COLOR, FPS_CLEAR_COLOR );
    const u32 frames = fps->total_frames, misses = fps->total_misses;
    const u32 ticks_ms = get_total_ms(), ticks = get_ticks();

    char *line1_fmt = "FPS: %05.2f, Frame: %u, Miss: %u";
    sprintf( fps_text, line1_fmt, fps->average_fps, frames, misses );
    graphics_draw_text( gfx->disp, 10, gfx->height - 33, fps_text );

    sprintf( fps_text, "Milli: %u, Tick: %u", ticks_ms, ticks );
    graphics_draw_text( gfx->disp, 10, gfx->height - 20, fps_text );
}
