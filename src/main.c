#include "system.h"
#include "audio.h"
#include "graphics.h"

#include "title.h"
#include "background.h"
#include "bird.h"
#include "pipes.h"

int main(void)
{
    /* Enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    dfs_init( DFS_ROM_LOCATION );
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

    /* Initialize game sprites */
    sprite_t *logo = read_dfs_sprite( "/gfx/logo.sprite" );
    sprite_t *headings = read_dfs_sprite( "/gfx/headings.sprite" );
    sprite_t *how_to = read_dfs_sprite( "/gfx/how-to.sprite" );

    /* Initialize game state */
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
                // collision_tick( &bird, &pipes );
                break;
        }

        /* Buffer sound effects */
        audio_tick( audio );

        /* Grab a display buffer and start drawing */
        graphics_display_lock( graphics );
        {
            /* Color fills */
            background_draw_color( graphics, bg.sky_fill );
            background_draw_color( graphics, bg.cloud_fill );
            background_draw_color( graphics, bg.hill_fill );
            background_draw_color( graphics, bg.ground_fill );

            /* Texture fills */
            background_draw_sprite( graphics, bg.cloud_top );
            background_draw_sprite( graphics, bg.city );
            background_draw_sprite( graphics, bg.hill_top );
            background_draw_sprite( graphics, bg.ground_top );

            /* Draw the pipes */
            pipes_draw( graphics, pipes );

            /* Draw the bird */
            bird_draw( graphics, bird );

            /* Draw the UI */
            // u16 score = bird.score;
            switch (bird.state)
            {
                case BIRD_STATE_TITLE:
                    logo_draw( graphics, logo );
                    break;
                case BIRD_STATE_READY:
                    heading_draw( graphics, headings, HEADING_GET_READY );
                    // score_draw( graphics, font_large, score );
                    howto_draw( graphics, how_to );
                    break;
                case BIRD_STATE_PLAY:
                    // score_draw( graphics, font_large, score );
                    break;
                case BIRD_STATE_DEAD:
                    heading_draw( graphics, headings, HEADING_GAME_OVER );
                    // scoreboard_draw( graphics, scoreboard );
                    // medal_draw( graphics, medals, score );
                    // scoreboard_scores_draw( graphics, font_med, score );
                    break;
            }
        }
        graphics_display_flip( graphics );
    }
}
