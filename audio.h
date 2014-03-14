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

/* Audio helpers */

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 rate, u8 channels);
void free_pcm_sound(pcm_sound_t *sfx);

#endif
