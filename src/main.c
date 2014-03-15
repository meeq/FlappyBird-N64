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
    audio_t *audio = audio_setup( FREQUENCY_44KHZ, 1 );
    audio_write_silence();

    pcm_sound_t *test_sound;
    test_sound = read_dfs_pcm_sound("/sfx/test.raw", audio->sample_rate, 1);
    audio_sfx_play( audio, test_sound );

    /* Set up main loop */

    u32 bg_color = graphics_make_color( 0xDD, 0xEE, 0xFF, 0xFF );
    u32 fg_color = graphics_make_color( 0x00, 0x00, 0x00, 0x00 );

    /* Run the main loop */
    while(1)
    {
        audio_tick( audio );

        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );

        /*Fill the screen */
        graphics_fill_screen( disp, bg_color );

        /* Set the text output color */
        graphics_set_color( fg_color, bg_color );

        /* Force backbuffer flip */
        display_show(disp);


    }
}
