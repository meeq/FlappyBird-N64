#ifndef __FLAPPY_FPS_H
#define __FLAPPY_FPS_H

#include "system.h"
#include "graphics.h"

/* FPS definitions */

#define FPS_MAX             60
#define FPS_FRAME_PERIOD    (1000 / FPS_MAX)
#define FPS_STAT_PERIOD     500
#define FPS_STATS_PER_SEC   (double) (FPS_STAT_PERIOD / 1000.0)
#define FPS_NUM_HISTORY     10

#define FPS_TEXT_COLOR      graphics_make_color( 0x00, 0x00, 0x00, 0xFF )
#define FPS_CLEAR_COLOR     graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

typedef struct
{
    bool should_draw;
    u64 frame_ms;
    u64 total_frames;
    u64 total_misses;
    u64 stat_ms;
    u8 frames_per_stat;
    u8 stat_count;
    double stat_fps[FPS_NUM_HISTORY];
    double average_fps;
} fps_counter_t;

/* FPS functions */

fps_counter_t fps_setup(void);

void fps_tick(fps_counter_t *fps, const gamepad_state_t gamepad);

void fps_draw(const fps_counter_t fps);

#endif
