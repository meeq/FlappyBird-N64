#ifndef __FLAPPY_AUDIO_H
#define __FLAPPY_AUDIO_H

#include "system.h"

/* Audio definitions */

#define FREQUENCY_44KHZ 44100
#define MONO_PCM_SAMPLE_SIZE sizeof(s16)
#define STEREO_PCM_SAMPLE_SIZE (2 * MONO_PCM_SAMPLE_SIZE)

/* Sound FX definitions */

#define SFX_NUM_SOUNDS 5
#define SFX_DIE 0
#define SFX_HIT 1
#define SFX_POINT 2
#define SFX_SWOOSH 3
#define SFX_WING 4

typedef struct
{
    u16 sample_rate;
    u8 channels;
    u32 frames;
    u32 samples;
    s16 *data;
} pcm_sound_t;

typedef struct
{
    // Setup state
    u16 sample_rate;
    u32 frames;
    s16 *buffer;
    pcm_sound_t *sfx_cache[SFX_NUM_SOUNDS];
    // Playback state
    u32 sfx_cursor;
    pcm_sound_t *sfx;
} audio_t;

/* Audio helpers */

audio_t *audio_setup(u16 sample_rate, u8 buffers);

void audio_free(audio_t *audio);

void audio_tick(audio_t *audio);

void audio_play_sfx(audio_t *audio, u8 sfx_index);

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 sample_rate, u8 channels);

#endif
