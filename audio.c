#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "audio.h"

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 rate, u8 channels)
{
    int fp = dfs_open( file );
    u32 size = dfs_size( fp );
    s16 *data = malloc( size );
    dfs_read( data, 1, size, fp );
    dfs_close( fp );
    u32 frames = size / MONO_PCM_SAMPLE_SIZE;
    pcm_sound_t *result = malloc( sizeof(pcm_sound_t) );
    result->sample_rate = rate;
    result->channels = channels;
    result->frames = frames;
    result->samples = frames / channels;
    result->data = data;
    return result;
}

void free_pcm_sound(pcm_sound_t *sfx)
{
    free( sfx->data );
    free( sfx );
}
