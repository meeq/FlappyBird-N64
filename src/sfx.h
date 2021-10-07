/**
 * FlappyBird-N64 - audio.h
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef __FLAPPY_SFX_H
#define __FLAPPY_SFX_H

#include "system.h"

/* Audio definitions */

#define FREQUENCY_44KHZ 44100

/* Sound FX definitions */

typedef enum sfx_id
{
    SFX_DIE = 0,
    SFX_HIT,
    SFX_POINT,
    SFX_SWOOSH,
    SFX_WING,
    SFX_ID_COUNT, // Not an actual ID, just a handy counter
} sfx_id_t;

/* Sound FX functions */

void sfx_init(void);

void sfx_close(void);

void sfx_play(const sfx_id_t sfx_id);

#endif
