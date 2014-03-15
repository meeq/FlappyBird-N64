#ifndef __FLAPPY_AUDIO_H
#define __FLAPPY_AUDIO_H

#include "system.h"

/* Audio definitions */

#define FREQUENCY_44KHZ 44100
#define MONO_PCM_SAMPLE_SIZE sizeof(s16)
#define STEREO_PCM_SAMPLE_SIZE (2 * MONO_PCM_SAMPLE_SIZE)

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
  // Playback state
  u32 sfx_cursor;
  pcm_sound_t *sfx;
} audio_t;

/* Audio helpers */

audio_t *audio_setup(u16 sample_rate, u8 buffers);

void free_audio(audio_t *audio);

void audio_sfx_play(audio_t *audio, pcm_sound_t *sfx);

void audio_tick(audio_t *audio);

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 sample_rate, u8 channels);

void free_pcm_sound(pcm_sound_t *sfx);

#endif
