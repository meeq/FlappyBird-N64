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
    s16 sky_fill_y = 0;
    s16 sky_fill_h = 141;

    sprite_t *cloud_sprite = read_dfs_sprite("/gfx/bg-cloud-day.sprite");
    s16 cloud_repeat_y = 119;

    u32 cloud_fill_color = graphics_make_color( 0xE9, 0xFC, 0xD9, 0xFF );
    s16 cloud_fill_y = 141;
    s16 cloud_fill_h = 22;

    sprite_t *city_sprite = read_dfs_sprite("/gfx/bg-city-day.sprite");
    s16 city_repeat_y = 148;

    sprite_t *hill_sprite = read_dfs_sprite("/gfx/bg-hill-day.sprite");
    s16 hill_repeat_y = 158;

    u32 hill_fill_color = graphics_make_color( 0x5E, 0xE2, 0x70, 0xFF );
    s16 hill_fill_y = 169;
    s16 hill_fill_h = 21;

    sprite_t *ground_sprite = read_dfs_sprite("/gfx/ground.sprite");
    s16 ground_repeat_y = 190;

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

        /* Ensure the RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Attach RDP to display */
        rdp_attach_display( disp );

        /* Prepare the background fills */
        rdp_enable_primitive_fill();

        /* Draw the sky fill */
        rdp_set_primitive_color( sky_fill_color );
        rdp_draw_filled_rectangle( 0,
            sky_fill_y, 320, sky_fill_y + sky_fill_h );

        /* Draw the cloud fill */
        rdp_set_primitive_color( cloud_fill_color );
        rdp_draw_filled_rectangle( 0,
            cloud_fill_y, 320, cloud_fill_y + cloud_fill_h );

        /* Draw the hill fill */
        rdp_set_primitive_color( hill_fill_color );
        rdp_draw_filled_rectangle( 0,
            hill_fill_y, 320, hill_fill_y + hill_fill_h );

        /* Draw the ground fill */
        rdp_set_primitive_color( ground_fill_color );
        rdp_draw_filled_rectangle( 0,
            ground_fill_y, 320, ground_fill_y + ground_fill_h );

        /* Enable sprite display instead of solid color fill */
        rdp_enable_texture_copy();

        /* Draw the cloud repeat sprite */
        for (int repeat_x = 0,
                 slices = cloud_sprite->hslices,
                 repeat_w = cloud_sprite->width / slices,
                 slice;
             repeat_x < 320; )
        {
            for (slice = 0;
                 slice < slices && repeat_x < 320;
                 slice++, repeat_x += repeat_w)
            {
                rdp_sync( SYNC_PIPE );
                rdp_load_texture_stride( 0, 0, MIRROR_DISABLED,
                    cloud_sprite, slice );
                rdp_draw_sprite( 0, repeat_x, cloud_repeat_y );
            }
        }

        /* Ensure the RDP is ready to receive sprites */
        rdp_sync( SYNC_PIPE );

        /* Draw the city repeat sprite */
        rdp_load_texture( 0, 0, MIRROR_DISABLED, city_sprite );
        rdp_draw_textured_rectangle( 0,
            0, city_repeat_y,
            320, city_repeat_y + city_sprite->height - 1);

        /* Ensure the RDP is ready to receive sprites */
        rdp_sync( SYNC_PIPE );

        /* Draw the hill repeat sprite */
        for (int repeat_x = 0,
                 slices = hill_sprite->hslices,
                 repeat_w = hill_sprite->width / slices,
                 slice;
             repeat_x < 320; )
        {
            for (slice = 0;
                 slice < slices && repeat_x < 320;
                 slice++, repeat_x += repeat_w)
            {
                rdp_sync( SYNC_PIPE );
                rdp_load_texture_stride( 0, 0, MIRROR_DISABLED,
                    hill_sprite, slice );
                rdp_draw_sprite( 0, repeat_x, hill_repeat_y );
            }
        }

        /* Ensure the RDP is ready to receive sprites */
        rdp_sync( SYNC_PIPE );

        /* Draw the ground repeat sprite */
        rdp_load_texture( 0, 0, MIRROR_DISABLED, ground_sprite );
        rdp_draw_textured_rectangle( 0,
            0, ground_repeat_y,
            320, ground_repeat_y + ground_sprite->height - 1);

        /* Inform the RDP drawing is finished; flush pending operations */
        rdp_detach_display();

        /* Force backbuffer flip */
        display_show( disp );
    }
}
