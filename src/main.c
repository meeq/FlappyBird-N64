#include "system.h"
#include "audio.h"
#include "graphics.h"

#include "title.h"
#include "background.h"
#include "bird.h"
#include "pipes.h"
#include "collision.h"

#include "global.h"

audio_t *g_audio = NULL;
graphics_t *g_graphics = NULL;

int main(void)
{
    /* Enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    dfs_init( DFS_ROM_LOCATION );
    controller_init();

    /* Initialize display */
    /* TODO Support 640x480 for 2-player splitscreen */
    g_graphics = graphics_setup(
        RESOLUTION_320x240, DEPTH_16_BPP,
        BUFFERING_DOUBLE, GAMMA_NONE, ANTIALIAS_RESAMPLE
    );

    /* Initialize audio */
    g_audio = audio_setup( FREQUENCY_44KHZ, 1 );
    audio_write_silence();

    /* Initialize game sprites */
    sprite_t *logo = read_dfs_sprite( "/gfx/logo.sprite" );
    sprite_t *headings = read_dfs_sprite( "/gfx/headings.sprite" );
    sprite_t *how_to = read_dfs_sprite( "/gfx/how-to.sprite" );
    sprite_t *font_large = read_dfs_sprite( "/gfx/font-large.sprite" );

    /* Initialize game state */
    u16 score = 0;
    background_t bg = background_setup( DAY_TIME );
    bird_t bird = bird_setup( BIRD_COLOR_YELLOW );
    pipes_t pipes = pipes_setup();

    /* Run the main loop */
    while(1)
    {
        /* Update controller state */
        controller_scan();
        struct controller_data keys = get_keys_down();

        /* Switch between day and night */
        if( keys.c[0].L )
        {
            background_free( bg );
            bg = background_setup( !bg.time_mode );
        }

        /* Update game state */
        bird_tick( &bird, keys.c[0] );
        switch (bird.state)
        {
            case BIRD_STATE_TITLE:
            case BIRD_STATE_READY:
                background_tick( &bg );
                pipes_reset( &pipes );
                break;
            case BIRD_STATE_PLAY:
                background_tick( &bg );
                pipes_tick( &pipes );
                collision_tick( &bird, &pipes, &score );
                break;
        }

        /* Buffer sound effects */
        audio_tick( g_audio );

        /* Grab a display buffer and start drawing */
        graphics_display_lock( g_graphics );
        {
            /* Color fills */
            background_draw_color( g_graphics, bg.sky_fill );
            background_draw_color( g_graphics, bg.cloud_fill );
            background_draw_color( g_graphics, bg.hill_fill );
            background_draw_color( g_graphics, bg.ground_fill );

            /* Texture fills */
            background_draw_sprite( g_graphics, bg.cloud_top );
            background_draw_sprite( g_graphics, bg.city );
            background_draw_sprite( g_graphics, bg.hill_top );
            background_draw_sprite( g_graphics, bg.ground_top );

            /* Draw the pipes */
            pipes_draw( g_graphics, pipes );

            /* Draw the bird */
            bird_draw( g_graphics, bird );

            /* Draw the UI */
            switch (bird.state)
            {
                case BIRD_STATE_TITLE:
                    logo_draw( g_graphics, logo, bg.time_mode );
                    break;
                case BIRD_STATE_READY:
                    score_draw( g_graphics, font_large, score );
                    heading_draw( g_graphics, headings, HEADING_GET_READY );
                    howto_draw( g_graphics, how_to );
                    break;
                case BIRD_STATE_PLAY:
                    score_draw( g_graphics, font_large, score );
                    break;
                case BIRD_STATE_DEAD:
                    heading_draw( g_graphics, headings, HEADING_GAME_OVER );
                    // scoreboard_draw( g_graphics, scoreboard );
                    // medal_draw( g_graphics, medals, score );
                    // scoreboard_scores_draw( g_graphics, font_med, score );
                    break;
            }
        }
        graphics_display_flip( g_graphics );
    }

    /* Clean up the initialized subsystems */
    audio_free( g_audio );
    g_audio = NULL;
    graphics_free( g_graphics );
    g_graphics = NULL;

    return 0;
}
