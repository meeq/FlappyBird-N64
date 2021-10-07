/**
 * FlappyBird-N64 - bird.h
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_BIRD_H
#define __FLAPPY_BIRD_H

#include "system.h"

/* Bird definitions */

typedef enum
{
    BIRD_STATE_READY,
    BIRD_STATE_PLAY,
    BIRD_STATE_DYING,
    BIRD_STATE_DEAD,
    BIRD_STATE_TITLE
} bird_state_t;

typedef enum
{
    BIRD_COLOR_YELLOW,
    BIRD_COLOR_RED,
    BIRD_COLOR_BLUE,
    BIRD_NUM_COLORS
} bird_color_t;

typedef struct bird_s
{
    sprite_t * sprite;
    int slice_w;
    int slice_h;
    bird_state_t state;
    bird_color_t color_type;
    int64_t hit_ms;
    int64_t dead_ms;
    bool is_rumbling;
    bool played_die_sfx;
    int score;
    /* Animation */
    int64_t anim_ms;
    int anim_frame;
    /* Center point */
    float x;
    float y;
    float dx;
    float dy;
    int64_t dy_ms;
    /* Ready "floating" wave */
    int64_t sine_ms;
    float sine_x;
    float sine_y;
} bird_t;

/* Bird functions */

bird_t * bird_init(bird_color_t color_type);

void bird_free(bird_t * bird);

void bird_draw(const bird_t * bird);

void bird_hit(bird_t * bird);

void bird_tick(bird_t * bird, const gamepad_state_t * gamepad);

#endif
