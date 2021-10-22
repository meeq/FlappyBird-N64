/**
 * FlappyBird-N64 - sfx.h
 *
 * Copyright 2021, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_SFX_H
#define __FLAPPY_SFX_H

/* Sound FX definitions */

typedef enum
{
    SFX_DIE = 0,
    SFX_HIT,
    SFX_POINT,
    SFX_SWOOSH,
    SFX_WING,
    // Additional sfx go above this line
    SFX_ID_COUNT, // Not an actual ID, just a handy counter
} sfx_id_t;

/* Sound FX functions */

void sfx_init(void);

void sfx_play(sfx_id_t sfx_id);

#endif
