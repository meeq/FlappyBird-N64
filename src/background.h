/**
 * FlappyBird-N64 - background.h
 *
 * Copyright 2021, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_BACKGROUND_H
#define __FLAPPY_BACKGROUND_H

#include "system.h"

/* Background definitions */

#define BG_SKY_FILL_Y           ((int) 0)
#define BG_SKY_FILL_H           ((int) 141)
#define BG_CLOUD_TOP_Y          ((int) 130)
#define BG_CLOUD_FILL_Y         ((int) 141)
#define BG_CLOUD_FILL_H         ((int) 32)
#define BG_CITY_TOP_Y           ((int) 155)
#define BG_HILL_TOP_Y           ((int) 170)
#define BG_HILL_FILL_Y          ((int) 179)
#define BG_HILL_FILL_H          ((int) 11)
#define BG_GROUND_TOP_Y         ((int) 190)
#define BG_GROUND_FILL_Y        ((int) 200)
#define BG_GROUND_FILL_H        ((int) 40)

typedef enum
{
    BG_DAY_TIME,
    BG_NIGHT_TIME,
    BG_NUM_TIME_MODES
} bg_time_mode_t;

typedef struct background_s background_t;

/* Background functions */

background_t * background_init(bg_time_mode_t time_mode);

void background_free(background_t * bg);

bg_time_mode_t background_get_time_mode(const background_t * bg);

void background_set_time_mode(background_t * bg, bg_time_mode_t time_mode);

void background_randomize_time_mode(background_t * bg);

void background_tick(background_t * bg, const gamepad_state_t * gamepad);

void background_draw(const background_t * bg);

#endif
