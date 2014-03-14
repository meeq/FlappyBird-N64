#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "system.h"
#include "audio.h"

sprite_t *read_dfs_sprite(char *file)
{
    int fp = dfs_open( file );
    sprite_t *sprite = malloc( dfs_size( fp ) );
    dfs_read( sprite, 1, dfs_size( fp ), fp );
    dfs_close( fp );
    return sprite;
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
    u16 sample_rate = FREQUENCY_44KHZ;
    audio_init( sample_rate, 1 );
    int audio_buff_length = audio_get_buffer_length() * STEREO_PCM_SAMPLE_SIZE;
    s16* audio_buff = malloc( audio_buff_length );
    audio_write_silence();

    pcm_sound_t *test_sound = read_dfs_pcm_sound("/sfx/test.raw", sample_rate, 1);
    int buffered = 0;

    /* Set up main loop */

    u32 bg_color = graphics_make_color( 0xDD, 0xEE, 0xFF, 0xFF );
    u32 fg_color = graphics_make_color( 0x00, 0x00, 0x00, 0x00 );

    char *test_string = "Trivial font/color/audio test";

    /* Run the main loop */
    while(1)
    {
        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );

        /*Fill the screen */
        graphics_fill_screen( disp, bg_color );

        /* Set the text output color */
        graphics_set_color( fg_color, bg_color );

        graphics_draw_text( disp, 20, 20, test_string );

        /* Force backbuffer flip */
        display_show(disp);

        if (audio_can_write())
        {
            sprintf(test_string, "Samples %i/%i", buffered, test_sound->samples);
            if (buffered < test_sound->samples)
            {
                s16 sample = 0;
                int limit = buffered + (audio_buff_length / STEREO_PCM_SAMPLE_SIZE);
                for (int i = 0; buffered < limit; buffered += 1, i += 2)
                {
                    if (buffered < test_sound->samples)
                    {
                        sample = test_sound->data[buffered];
                    } else {
                        sample = 0;
                    }
                    audio_buff[i] = audio_buff[i + 1] = sample;
                }
                audio_write( audio_buff );
            } else {
                audio_write_silence();
            }
        }
    }
}
