#ifndef __FLAPPY_AUDIO_H
#define __FLAPPY_AUDIO_H

#include "system.h"

/* Audio definitions */

#define FREQUENCY_44KHZ 44100
#define MONO_PCM_SAMPLE_SIZE sizeof(s16)
#define STEREO_PCM_SAMPLE_SIZE (MONO_PCM_SAMPLE_SIZE << 1)

/* Sound FX definitions */

#define SFX_NUM_CHANNELS    4
#define SFX_NUM_SOUNDS      5

typedef enum sfx_sounds
{
    SFX_DIE,
    SFX_HIT,
    SFX_POINT,
    SFX_SWOOSH,
    SFX_WING
} sfx_sounds_t;

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
    u32 cursor;
    pcm_sound_t *sfx;
} sfx_channel_t;

typedef struct
{
    // Setup state
    u16 sample_rate;
    u32 frames;
    s16 *buffer;
    pcm_sound_t *sfx_cache[SFX_NUM_SOUNDS];
    // Playback state
    sfx_channel_t channels[SFX_NUM_CHANNELS];
} audio_t;

/* Audio helpers */

audio_t *audio_setup(u16 sample_rate, u8 buffers);

void audio_free(audio_t *audio);

void audio_tick(audio_t *audio);

void audio_play_sfx(audio_t *audio, sfx_sounds_t sfx_sound);

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 sample_rate, u8 channels);

#endif
