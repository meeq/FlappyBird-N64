/**
 * FlappyBird-N64 - bird.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_BIRD_H
#define __FLAPPY_BIRD_H

#include <libdragon.h>

/* Bird definitions */

typedef enum
{
    BIRD_STATE_READY,
    BIRD_STATE_PLAY,
    BIRD_STATE_DYING,
    BIRD_STATE_DEAD,
    BIRD_STATE_TITLE,
} bird_state_t;

typedef enum
{
    BIRD_COLOR_YELLOW,
    BIRD_COLOR_RED,
    BIRD_COLOR_BLUE,
    // Additional colors go above this line
    BIRD_COLORS_COUNT, // Not a color; just a count
} bird_color_t;

typedef struct bird_s
{
    sprite_t *sprite;
    int slice_w;
    int slice_h;
    bird_state_t state;
    bird_color_t color_type;
    uint32_t hit_ticks;
    uint32_t dead_ticks;
    bool is_dead_reset;
    bool is_rumbling;
    bool played_die_sfx;
    int score;
    /* Animation */
    uint32_t anim_ticks;
    int anim_frame;
    /* Center point */
    float x;
    float y;
    float dx;
    float dy;
    uint32_t dy_ticks;
    /* Ready "floating" wave */
    uint32_t sine_ticks;
    float sine_x;
    float sine_y;
    /* Rotation */
    float rotation;
    uint32_t flap_ticks;
} bird_t;

/* Bird functions */

bird_t *bird_init(bird_color_t color_type);

void bird_free(bird_t *bird);

void bird_draw(const bird_t *bird);

void bird_hit(bird_t *bird);

void bird_tick(bird_t *bird, const joypad_buttons_t *buttons);

#endif
