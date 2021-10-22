/**
 * FlappyBird-N64 - pipes.h
 *
 * Copyright 2021, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_PIPES_H
#define __FLAPPY_PIPES_H

#include "system.h"

#define PIPES_MAX_NUM   5

typedef enum
{
    PIPE_COLOR_GREEN,
    PIPE_COLOR_RED,
    PIPE_NUM_COLORS
} pipe_colors_t;

typedef struct pipe_s
{
    float x;
    float y; /* Min: -1.0   Max: 1.0 */
    bool has_scored;
} pipe_t;

typedef struct pipes_s
{
    pipe_colors_t color;
    int64_t reset_ms;
    int64_t scroll_ms;
    sprite_t * cap_sprite;
    sprite_t * tube_sprite;
    pipe_t n[PIPES_MAX_NUM];
} pipes_t;

/* Pipes functions */

pipes_t * pipes_init(void);

void pipes_free(pipes_t * pipes);

void pipes_reset(pipes_t * pipes);

void pipes_tick(pipes_t * pipes);

void pipes_draw(const pipes_t * pipes);

#endif
