#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "system.h"

sprite_t *read_dfs_sprite(char *file)
{
    int fp = dfs_open( file );
    sprite_t *sprite = malloc( dfs_size( fp ) );
    dfs_read( sprite, 1, dfs_size( fp ), fp );
    dfs_close( fp );
    return sprite;
}

raw_pcm_t read_dfs_raw_pcm(char *file)
{
    int fp = dfs_open( file );
    int size = dfs_size( fp );
    s16 *data = malloc( size );
    dfs_read( data, 1, size, fp );
    dfs_close( fp );
    raw_pcm_t sound;
    sound.numSamples = size / sizeof(u16);
    sound.sampleData = data;
    return sound;
}

int main(void)
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init(
        RESOLUTION_320x240, DEPTH_16_BPP,
        BUFFERING_DOUBLE, GAMMA_NONE, ANTIALIAS_RESAMPLE
    );
    rdp_init();
    controller_init();
    timer_init();
    dfs_init( DFS_DEFAULT_LOCATION );

    /* Initialize audio */
    audio_init( FREQUENCY_44KHZ, 1 );
    int audioBuffSize = audio_get_buffer_length() * STEREO_PCM_SAMPLE_SIZE;
    s16* audioBuff = malloc( audioBuffSize );
    audio_write_silence();

    raw_pcm_t sound = read_dfs_raw_pcm("/sfx/test.raw");
    int samplesBuffered = 0;

    /* Set up main loop */

    u32 bgColor = graphics_make_color( 0xDD, 0xEE, 0xFF, 0xFF );
    u32 fgColor = graphics_make_color( 0x00, 0x00, 0x00, 0x00 );

    char *testString = "Trivial font/color/audio test";

    /* Run the main loop */
    while(1)
    {
        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );

        /*Fill the screen */
        graphics_fill_screen( disp, bgColor );

        /* Set the text output color */
        graphics_set_color( fgColor, bgColor );

        graphics_draw_text( disp, 20, 20, testString );

        /* Force backbuffer flip */
        display_show(disp);

        if (audio_can_write())
        {
             sprintf(testString, "Samples %i/%i", samplesBuffered, sound.numSamples);
            if (samplesBuffered < sound.numSamples)
            {
                s16 sample = 0;
                int writeSamples = samplesBuffered + (audioBuffSize / STEREO_PCM_SAMPLE_SIZE);
                for (int i = 0; samplesBuffered < writeSamples; samplesBuffered += 1, i += 2)
                {
                    sample = (samplesBuffered < sound.numSamples) ? sound.sampleData[samplesBuffered] : 0;
                    audioBuff[i] = audioBuff[i + 1] = sample;
                }
                audio_write(audioBuff);
            } else {
                audio_write_silence();
            }
        }
    }
}
