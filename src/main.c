#include "system.h"
#include "audio.h"
#include "graphics.h"


int main(void)
{
    /* Enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    dfs_init( DFS_DEFAULT_LOCATION );
    controller_init();

    /* Initialize display */
    graphics_t *graphics = graphics_setup(
        RESOLUTION_320x240, DEPTH_16_BPP,
        BUFFERING_DOUBLE, GAMMA_NONE, ANTIALIAS_RESAMPLE
    );

    /* Initialize audio */
    audio_t *audio = audio_setup( FREQUENCY_44KHZ, 1 );
    audio_write_silence();

    /* Set up the background fills from top-to-bottom */
    /* These are all hard-coded for 320x240 resolution... */
    x_fill_color_t sky = {
        .color = graphics_make_color( 0x4E, 0xC0, 0xCA, 0xFF ),
        .y = 0, .h = 141
    };
    x_fill_sprite_t cloud_top = {
        .sprite = read_dfs_sprite("/gfx/bg-cloud-day.sprite"),
        .y = 119
    };
    x_fill_color_t cloud_fill = {
        .color = graphics_make_color( 0xE9, 0xFC, 0xD9, 0xFF ),
        .y = 141, .h = 22
    };
    x_fill_sprite_t city = {
        .sprite = read_dfs_sprite("/gfx/bg-city-day.sprite"),
        .y = 148
    };
    x_fill_sprite_t hill_top = {
        .sprite = read_dfs_sprite("/gfx/bg-hill-day.sprite"),
        .y = 158
    };
    x_fill_color_t hill_fill = {
        .color = graphics_make_color( 0x5E, 0xE2, 0x70, 0xFF ),
        .y = 169, .h = 21
    };
    x_fill_sprite_t ground_top = {
        .sprite = read_dfs_sprite("/gfx/ground.sprite"),
        .y = 190
    };
    x_fill_color_t ground_fill = {
        .color = graphics_make_color( 0xDF, 0xD8, 0x93, 0xFF ),
        .y = 200, .h = 40
    };

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
        draw_x_fill_color(graphics, sky);
        draw_x_fill_color(graphics, cloud_fill);
        draw_x_fill_color(graphics, hill_fill);
        draw_x_fill_color(graphics, ground_fill);

        /* Texture fills */
        draw_x_fill_sprite(graphics, cloud_top);
        draw_x_fill_sprite(graphics, city);
        draw_x_fill_sprite(graphics, hill_top);
        draw_x_fill_sprite(graphics, ground_top);

        /* Inform the RDP drawing is finished; flush pending operations */
        rdp_detach_display();
        graphics->rdp_attached = RDP_DETACHED;
        graphics->rdp_fill_mode = RDP_FILL_NONE;

        /* Force backbuffer flip and reset the display handle */
        display_show( disp );
        graphics->disp = disp = 0;
    }
}
