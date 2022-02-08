/**
 * FlappyBird-N64 - background.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_BACKGROUND_H
#define __FLAPPY_BACKGROUND_H

#include "system.h"

/* Background constants */

#define BG_GROUND_TOP_Y         ((int)190)

/* Background types */

typedef enum
{
    BG_TIME_DAY,
    BG_TIME_NIGHT,
    // Additional modes go above this line
    BG_TIME_MODES_COUNT // Not a mode; just a count
} bg_time_mode_t;

typedef struct background_s background_t;

/* Background functions */

background_t *background_init(bg_time_mode_t time_mode);

void background_free(background_t *bg);

bg_time_mode_t background_get_time_mode(const background_t *bg);

void background_set_time_mode(background_t *bg, bg_time_mode_t time_mode);

void background_randomize_time_mode(background_t *bg);

void background_tick(background_t *bg, const gamepad_state_t *gamepad);

void background_draw(const background_t *bg);

#endif
