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
#include "graphics.h"

/* Bird definitions */

/* States */
typedef enum bird_state
{
    BIRD_STATE_READY,
    BIRD_STATE_PLAY,
    BIRD_STATE_DYING,
    BIRD_STATE_DEAD,
    BIRD_STATE_TITLE
} bird_state_t;

#define BIRD_RESET_DELAY    1000
#define BIRD_RUMBLE_MS      500

/* Colors */
typedef enum bird_color
{
    BIRD_COLOR_YELLOW,
    BIRD_COLOR_RED,
    BIRD_COLOR_BLUE,
    BIRD_NUM_COLORS
} bird_color_t;

/* Animation */
#define BIRD_ANIM_RATE      120
#define BIRD_ANIM_FRAMES    3
#define BIRD_DYING_FRAME    3

/* Center point */
#define BIRD_TITLE_X        0.5
#define BIRD_PLAY_X         0.35
#define BIRD_ACCEL_X        0.001
#define BIRD_MIN_Y          -0.90
#define BIRD_MAX_Y          0.95

/* Flap */
#define BIRD_VELOCITY_RATE  16
#define BIRD_FLAP_VELOCITY  0.0270
#define BIRD_GRAVITY_ACCEL  0.0013

/* Sine "floating" effect */
#define BIRD_SINE_RATE      20
#define BIRD_SINE_INCREMENT 0.1
#define BIRD_SINE_CYCLE     (M_PI * 2.0)
#define BIRD_SINE_DAMPEN    0.02

typedef struct
{
    sprite_t *sprite;
    u8 slice_w;
    u8 slice_h;
    bird_state_t state;
    bird_color_t color_type;
    u32 hit_ms;
    u32 dead_ms;
    bool is_rumbling;
    bool played_die_sfx;
    u16 score;
    /* Animation */
    u32 anim_ms;
    u8 anim_frame;
    /* Center point */
    float x;
    float y;
    float dx;
    float dy;
    u32 dy_ms;
    /* Ready "floating" wave */
    u32 sine_ms;
    float sine_x;
    float sine_y;
} bird_t;

/* Bird functions */

bird_t bird_setup(bird_color_t color_type);

void bird_free(bird_t *bird);

void bird_draw(const bird_t *bird);

void bird_hit(bird_t *bird);

void bird_tick(bird_t *bird, const gamepad_state_t *gamepad);

#endif
