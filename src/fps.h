/**
 * FlappyBird-N64 - fps.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_FPS_H
#define __FLAPPY_FPS_H

#include "system.h"

/* FPS functions */

void fps_init(void);

void fps_tick(const gamepad_state_t * const gamepad);

void fps_draw(void);

#endif
