#include "system.h"
#include "audio.h"
#include "graphics.h"

#include "background.h"

int main(void)
{
    /* Enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    dfs_init( DFS_DEFAULT_LOCATION );
    controller_init();

    /* Initialize display */
    /* TODO Support 640x480 for 2-player splitscreen */
    graphics_t *graphics = graphics_setup(
        RESOLUTION_320x240, DEPTH_16_BPP,
        BUFFERING_DOUBLE, GAMMA_NONE, ANTIALIAS_RESAMPLE
    );

    /* Initialize audio */
    audio_t *audio = audio_setup( FREQUENCY_44KHZ, 1 );
    audio_write_silence();

    background_t bg = background_setup( DAY_TIME );

    /* Run the main loop */
    while(1)
    {
        audio_tick( audio );

        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
        graphics->disp = disp;

        /* Ensure the RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Attach RDP to display */
        rdp_attach_display( disp );
        graphics->rdp_attached = RDP_ATTACHED;

        /* Color fills */
        draw_bg_fill_color( graphics, bg.sky_fill );
        draw_bg_fill_color( graphics, bg.cloud_fill );
        draw_bg_fill_color( graphics, bg.hill_fill );
        draw_bg_fill_color( graphics, bg.ground_fill );

        /* Texture fills */
        draw_bg_fill_sprite( graphics, bg.cloud_top );
        draw_bg_fill_sprite( graphics, bg.city );
        draw_bg_fill_sprite( graphics, bg.hill_top );
        draw_bg_fill_sprite( graphics, bg.ground_top );

        /* Inform the RDP drawing is finished; flush pending operations */
        rdp_detach_display();
        graphics->rdp_attached = RDP_DETACHED;
        graphics->rdp_fill_mode = RDP_FILL_NONE;

        /* Force backbuffer flip and reset the display handle */
        display_show( disp );
        graphics->disp = disp = 0;

        /* Do we need to switch backgrounds? */
        controller_scan();
        struct controller_data keys = get_keys_down();

        if( keys.c[0].A )
        {
            background_free( bg );
            bg = background_setup( !bg.time_mode );
        }
    }
}
