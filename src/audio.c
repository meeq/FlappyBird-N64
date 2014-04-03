#include "audio.h"

audio_t *audio_setup(u16 sample_rate, u8 buffers)
{
    /* Start up the audio subsystem */
    audio_init( sample_rate, buffers );
    int buffer_length = audio_get_buffer_length();
    s16 *buffer = malloc( buffer_length * STEREO_PCM_SAMPLE_SIZE );
    audio_t *audio = malloc( sizeof( audio_t ) );
    audio->sample_rate = sample_rate;
    audio->frames = buffer_length * 2;
    audio->buffer = buffer;
    audio->sfx_cursor = 0;
    audio->sfx = NULL;
    /* Load the sound effects cache */
    char *sfx_files[SFX_NUM_SOUNDS] = {
        "/sfx/die.raw",
        "/sfx/hit.raw",
        "/sfx/point.raw",
        "/sfx/swoosh.raw",
        "/sfx/wing.raw"
    };
    pcm_sound_t *sfx;
    for (int i = 0; i < SFX_NUM_SOUNDS; i++)
    {
        sfx = read_dfs_pcm_sound( sfx_files[i], sample_rate, 1);
        audio->sfx_cache[i] = sfx;
    }
    return audio;
}

void audio_free(audio_t *audio)
{
    /* Clear the sound effects cache */
    for (int i = 0; i < SFX_NUM_SOUNDS; i++)
    {
        free(audio->sfx_cache[i]->data);
        free(audio->sfx_cache[i]);
        audio->sfx_cache[i] = NULL;
    }
    /* Shut down the audio subsystem */
    free( audio->buffer );
    free( audio );
    audio_close();
}

void audio_tick(audio_t *audio)
{
    if (audio_can_write())
    {
        pcm_sound_t *sfx = audio->sfx;
        if (sfx != NULL)
        {
            u32 sfx_cursor = audio->sfx_cursor;
            u32 sfx_length = sfx->samples;
            if (sfx_cursor < sfx_length)
            {
                u32 buffer_frames = audio->frames;
                s16 *buffer = audio->buffer;
                u8  sfx_channels = sfx->channels;
                s16 *sfx_data = sfx->data;
                s16 sfx_sample = 0;
                // Fill the audio buffer with stereo sample frames
                for (int i = 0; i < buffer_frames; sfx_sample = 0)
                {
                    // Play silence if we're past the end of the data
                    if (sfx_cursor < sfx_length)
                    {
                        // Copy sound effect sample and advance the cursor
                        sfx_sample = sfx_data[sfx_cursor++];
                    }
                    // For stereo, alternate between left and right channels
                    buffer[i++] = sfx_sample;
                    // For mono, copy the left channel to the right
                    if (sfx_channels == 1)
                    {
                        // Pan the mono signal into stereo at full strength
                        buffer[i++] = sfx_sample;
                    }
                }
                audio->sfx_cursor = sfx_cursor;
                audio_write( buffer );
            } else {
                audio_write_silence();
            }
        } else {
            audio_write_silence();
        }
    }
}

void audio_play_sfx(audio_t *audio, u8 sfx_index)
{
    if (audio != NULL && sfx_index < SFX_NUM_SOUNDS)
    {
        pcm_sound_t *sfx = audio->sfx_cache[sfx_index];
        if (sfx != NULL)
        {
            audio->sfx_cursor = 0;
            audio->sfx = sfx;
        }
    }
}

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 sample_rate, u8 channels)
{
    int fp = dfs_open( file );
    u32 size = dfs_size( fp );
    s16 *data = malloc( size );
    dfs_read( data, 1, size, fp );
    dfs_close( fp );
    u32 frames = size / MONO_PCM_SAMPLE_SIZE;
    pcm_sound_t *sound = malloc( sizeof( pcm_sound_t ) );
    sound->sample_rate = sample_rate;
    sound->channels = channels;
    sound->frames = frames;
    sound->samples = frames / channels;
    sound->data = data;
    return sound;
}
