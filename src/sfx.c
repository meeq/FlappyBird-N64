/**
 * FlappyBird-N64 - sfx.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include "sfx.h"

#include "system.h"

#define SFX_SAMPLE_RATE 44100
#define SFX_NUM_BUFFERS 4

static wav64_t SFX_CACHE[SFX_ID_COUNT];

// This array must line up with sfx_id_t
static const char *const SFX_FILES[SFX_ID_COUNT] = {
    "sfx/die.wav64",
    "sfx/hit.wav64",
    "sfx/point.wav64",
    "sfx/swoosh.wav64",
    "sfx/wing.wav64",
};

void sfx_init(void)
{
    audio_init(SFX_SAMPLE_RATE, SFX_NUM_BUFFERS);
    audio_write_silence();
    mixer_init(SFX_ID_COUNT);

    /* Load the sound effects cache */
    for (size_t i = 0; i < SFX_ID_COUNT; i++)
    {
        wav64_open(&SFX_CACHE[i], SFX_FILES[i]);
    }
}

void sfx_play(sfx_id_t sfx_id)
{
    mixer_ch_play(sfx_id, &SFX_CACHE[sfx_id].wave);
}
