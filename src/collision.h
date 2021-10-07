/**
 * FlappyBird-N64 - collision.h
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_COLLISION_H
#define __FLAPPY_COLLISION_H

typedef struct bird_s bird_t;
typedef struct pipes_s pipes_t;

void collision_tick(bird_t * bird, pipes_t * pipes);

#endif
