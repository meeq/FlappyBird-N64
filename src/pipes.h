/**
 * FlappyBird-N64 - pipes.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_PIPES_H
#define __FLAPPY_PIPES_H

#include "system.h"

#define PIPES_MAX_COUNT 5

typedef enum
{
    PIPE_COLOR_GREEN,
    PIPE_COLOR_RED,
    // Additional colors go above this line
    PIPE_COLORS_COUNT // Not a color; just a count
} pipe_color_t;

typedef struct pipe_s
{
    float x;
    float y; /* (-1.0, +1.0) */
    bool has_scored;
} pipe_t;

typedef struct pipes_s
{
    pipe_color_t color;
    uint32_t reset_ticks;
    uint32_t scroll_ticks;
    sprite_t *cap_sprite;
    sprite_t *tube_sprite;
    pipe_t n[PIPES_MAX_COUNT];
} pipes_t;

/* Pipes functions */

pipes_t *pipes_init(void);

void pipes_free(pipes_t *pipes);

void pipes_reset(pipes_t *pipes);

void pipes_tick(pipes_t *pipes);

void pipes_draw(const pipes_t *pipes);

#endif
