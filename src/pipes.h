#ifndef __FLAPPY_PIPES_H
#define __FLAPPY_PIPES_H

#include "system.h"
#include "graphics.h"

#define PIPES_MAX_NUM       5

#define PIPES_SCROLL_RATE   16
#define PIPES_SCROLL_DX     -0.00312

typedef enum pipe_colors
{
    PIPE_GREEN_COLOR,
    PIPE_RED_COLOR
} pipe_colors_t;

#define PIPE_TUBE_WIDTH     26
#define PIPE_CAP_HEIGHT     13
#define PIPE_GAP_Y          80
#define PIPE_GAP_X          0.3
#define PIPE_START_X        1.1
#define PIPE_MIN_X          -0.1

typedef struct
{
    float x;
    float y; /* Min: -1.0   Max: 1.0 */
} pipe_t;

typedef struct
{
    pipe_colors_t color;
    u64 reset_ms;
    u64 scroll_ms;
    pipe_t n[PIPES_MAX_NUM];
    sprite_t *cap_sprite;
    sprite_t *tube_sprite;
} pipes_t;

pipes_t pipes_setup(void);

void pipes_free(pipes_t *pipes);

void pipes_reset(pipes_t *pipes);

void pipes_tick(pipes_t *pipes);

void pipes_draw(const pipes_t pipes);

#endif
