/**
 * FlappyBird-N64 - bg.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_BG_H
#define __FLAPPY_BG_H

#include <libdragon.h>

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

/* Background functions */

void bg_init(void);

bg_time_mode_t bg_get_time_mode(void);

void bg_set_time_mode(bg_time_mode_t time_mode);

void bg_randomize_time_mode(void);

void bg_tick(const joypad_buttons_t *buttons);

void bg_draw(void);

#endif
