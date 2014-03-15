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

    /* Set up main loop */

    u32 sky_fill_color = graphics_make_color( 0x4E, 0xC0, 0xCA, 0xFF );

    sprite_t *cloud_sprite = read_dfs_sprite("/gfx/bg-clouds-day.sprite");
    s16 cloud_repeat_y = 133;
    s16 cloud_repeat_h = 11;

    u32 cloud_fill_color = graphics_make_color( 0xE9, 0xFC, 0xD9, 0xFF );
    s16 cloud_fill_y = 141;
    s16 cloud_fill_h = 22;

    sprite_t *city_sprite = read_dfs_sprite("/gfx/bg-city-day.sprite");
    s16 city_repeat_y = 146;
    s16 city_repeat_h = 18;

    sprite_t *hill_sprite = read_dfs_sprite("/gfx/bg-hill-day.sprite");
    s16 hill_repeat_y = 160;
    s16 hill_repeat_h = 9;

    u32 hill_fill_color = graphics_make_color( 0x5E, 0xE2, 0x70, 0xFF );
    s16 hill_fill_y = 169;
    s16 hill_fill_h = 21;

    sprite_t *ground_sprite = read_dfs_sprite("/gfx/ground.sprite");
    s16 ground_repeat_y = 190;
    s16 ground_repeat_h = 11;

    u32 ground_fill_color = graphics_make_color( 0xDF, 0xD8, 0x93, 0xFF );
    s16 ground_fill_y = 200;
    s16 ground_fill_h = 40;

    /* Run the main loop */
    while(1)
    {
        audio_tick( audio );

        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );

        /* Draw the sky fill */
        graphics_fill_screen( disp, sky_fill_color );

        /* Draw the cloud fill */
        graphics_draw_box( disp,
            0, cloud_fill_y,
            320, cloud_fill_h,
            cloud_fill_color);

        /* Draw the hill fill */
        graphics_draw_box( disp,
            0, hill_fill_y,
            320, hill_fill_h,
            hill_fill_color);

        /* Draw the ground fill */
        graphics_draw_box( disp,
            0, ground_fill_y,
            320, ground_fill_h,
            ground_fill_color);

        /* Assure RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Enable sprite display instead of solid color fill */
        rdp_enable_texture_copy();

        /* Attach RDP to display */
        rdp_attach_display( disp );

        /* Ensure the RDP is ready to receive sprites */
        rdp_sync( SYNC_PIPE );

        /* Load and draw the cloud repeat sprite */
        rdp_load_texture( 0, 0, MIRROR_DISABLED, cloud_sprite );
        rdp_draw_textured_rectangle( 0,
            0, cloud_repeat_h,
            320, cloud_repeat_h + cloud_repeat_y);

        /* Ensure the RDP is ready to receive sprites */
        rdp_sync( SYNC_PIPE );

        /* Load and draw the city repeat sprite */
        rdp_load_texture( 0, 0, MIRROR_DISABLED, city_sprite );
        rdp_draw_textured_rectangle( 0,
            0, city_repeat_h,
            320, city_repeat_h + city_repeat_y);

        /* Ensure the RDP is ready to receive sprites */
        rdp_sync( SYNC_PIPE );

        /* Load and draw the hill repeat sprite */
        rdp_load_texture( 0, 0, MIRROR_DISABLED, hill_sprite );
        rdp_draw_textured_rectangle( 0,
            0, hill_repeat_h,
            320, hill_repeat_h + hill_repeat_y);

        /* Ensure the RDP is ready to receive sprites */
        rdp_sync( SYNC_PIPE );

        /* Load and draw the ground repeat sprite */
        rdp_load_texture( 0, 0, MIRROR_DISABLED, ground_sprite );
        rdp_draw_textured_rectangle( 0,
            0, ground_repeat_h,
            320, ground_repeat_h + ground_repeat_y);

        /* Inform the RDP we are finished drawing; flush pending operations */
        rdp_detach_display();

        /* Force backbuffer flip */
        display_show(disp);
    }
}