#include "fps.h"

#include "global.h"

fps_counter_t fps_setup(void)
{
    fps_counter_t fps = {
        .should_draw = FALSE,
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

void fps_tick(fps_counter_t *fps, const gamepad_state_t gamepad)
{
    /* Toggle drawing flag on C-up */
    if ( gamepad.C_up )
    {
        fps->should_draw = !fps->should_draw;
    }

    /* Increment frame counters */
    fps->total_frames++;
    fps->frames_per_stat++;

    /* Check timing */
    const u64 ticks_ms = get_ticks_ms();
    const s64 frame_diff_ms = ticks_ms - fps->frame_ms;
    if ( fps->total_frames > 1 && frame_diff_ms > FPS_FRAME_PERIOD )
    {
        s64 frame_period_diff = ticks_ms - fps->frame_ms - FPS_FRAME_PERIOD;
        fps->total_misses += frame_period_diff / FPS_FRAME_PERIOD;
    }
    const s64 stat_diff_ms = ticks_ms - fps->stat_ms;
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
        s64 stat_period_diff = ticks_ms - fps->stat_ms - FPS_STAT_PERIOD;
        fps->stat_ms = ticks_ms - stat_period_diff;
    }
    fps->frame_ms = ticks_ms;
}

void fps_draw(const fps_counter_t fps)
{
    if ( !fps.should_draw ) return;

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    char fps_text[32];
    graphics_set_color( FPS_TEXT_COLOR, FPS_CLEAR_COLOR );

    sprintf( fps_text, "FPS: %05.2f", fps.average_fps );
    graphics_draw_text( disp, 10, g_graphics->height - 33, fps_text );

    u32 frames = fps.total_frames, misses = fps.total_misses;
    sprintf( fps_text, "Total Frames: %d, Misses: %d", frames, misses );
    graphics_draw_text( disp, 10, g_graphics->height - 20, fps_text );
}
