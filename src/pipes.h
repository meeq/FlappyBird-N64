#ifndef __FLAPPY_PIPES_H
#define __FLAPPY_PIPES_H

#include "system.h"
#include "graphics.h"

#define PIPES_MAX_NUM       1

#define PIPES_SCROLL_RATE   16
#define PIPES_SCROLL_DX     -0.005

#define PIPE_GREEN_COLOR    0
#define PIPE_RED_COLOR      1

#define PIPE_WIDTH          26
#define PIPE_CAP_HEIGHT     13
#define PIPE_GAP_Y          80

typedef struct
{
    float x;
    float y;
} pipe_t;

typedef struct
{
    u8 color;
    u64 reset_ms;
    u64 scroll_ms;
    pipe_t n[PIPES_MAX_NUM];
    sprite_t *cap_sprite;
    sprite_t *tube_sprite;
} pipes_t;

pipes_t pipes_setup(void);

void pipes_free(pipes_t pipes);

void pipes_reset(pipes_t *pipes);

void pipes_tick(pipes_t *pipes);

void pipes_draw(graphics_t *graphics, pipes_t pipes);

#endif
