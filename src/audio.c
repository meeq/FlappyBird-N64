#include "audio.h"

audio_t *audio_setup(u16 sample_rate, u8 buffers)
{
    /* Start up the audio subsystem */
    audio_init( sample_rate, buffers );
    const int buffer_length = audio_get_buffer_length();
    s16 *buffer = malloc( buffer_length * STEREO_PCM_SAMPLE_SIZE );
    audio_t *audio = malloc( sizeof( audio_t ) );
    audio->sample_rate = sample_rate;
    audio->frames = buffer_length << 1;
    audio->buffer = buffer;
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
    /* Setup the sound effects channels */
    for (int i = 0; i < SFX_NUM_CHANNELS; i++)
    {
        audio->channels[i].cursor = 0;
        audio->channels[i].sfx = NULL;
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
    /* Clear sound effects pointers from playback channels */
    for (int i = 0; i < SFX_NUM_CHANNELS; i++)
    {
        audio->channels[i].sfx = NULL;
    }
    /* Shut down the audio subsystem */
    free( audio->buffer );
    free( audio );
    audio_close();
}

void audio_tick(audio_t *audio)
{
    if ( audio != NULL && audio_can_write() )
    {
        sfx_channel_t channel;
        pcm_sound_t *sfx;
        s32 left_mix, right_mix;
        u8 num_left, num_right;
        s16 left_sample, right_sample;
        /* Fill the audio buffer with stereo sample frames */
        for (int frame = 0; frame < audio->frames;
             left_mix = num_left = right_mix = num_right = 0)
        {
            /* Accumulate all currently playing sound effects samples */
            for (int i = 0; i < SFX_NUM_CHANNELS; i++)
            {
                channel = audio->channels[i];
                sfx = channel.sfx;
                if ( sfx != NULL && channel.cursor < sfx->samples )
                {
                    left_mix += sfx->data[channel.cursor++];
                    num_left++;
                    /* Play mono sound effects in both speakers */
                    if ( sfx->channels == 1 )
                    {
                        right_mix += sfx->data[channel.cursor - 1];
                        num_right++;
                    }
                    /* Play stereo sound effects in separate speakers */
                    else if ( channel.cursor < sfx->samples )
                    {
                        right_mix += sfx->data[channel.cursor++];
                        num_right++;
                    }
                    /* Reset channels that have finished playing */
                    if ( channel.cursor >= sfx->samples )
                    {
                        channel.cursor = 0;
                        channel.sfx = NULL;
                    }
                }
                audio->channels[i] = channel;
            }
            /* Mix down all of the samples as an average */
            left_sample = (num_left > 1) ? left_mix / num_left : left_mix;
            right_sample = (num_right > 1) ? right_mix / num_right : right_mix;
            audio->buffer[frame++] = left_sample;
            audio->buffer[frame++] = right_sample;
        }
        audio_write( audio->buffer );
    }
}

void audio_play_sfx(audio_t *audio, sfx_sounds_t sfx_sound)
{
    if ( audio != NULL )
    {
        pcm_sound_t *sfx = audio->sfx_cache[sfx_sound];
        if ( sfx != NULL )
        {
            for (int i = 0; i < SFX_NUM_CHANNELS; i++)
            {
                if ( audio->channels[i].sfx == NULL )
                {
                    audio->channels[i].sfx = sfx;
                    audio->channels[i].cursor = 0;
                    break;
                }
            }
        }
    }
}

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 sample_rate, u8 channels)
{
    int fp = dfs_open( file );
    const u32 size = dfs_size( fp );
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
