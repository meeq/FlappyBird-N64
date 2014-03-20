#include "system.h"
#include "audio.h"
#include "graphics.h"

#include "background.h"
#include "bird.h"

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

    /* Initialize game state */
    background_t bg = background_setup( DAY_TIME );
    bird_t bird = bird_setup( BIRD_COLOR_YELLOW );
    // pipes_t pipes = pipes_setup();

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
            // background_toggle_time_mode( &bg );
        }

        /* Update game state */
        bird_tick( &bird, keys.c[0] );
        switch (bird.state)
        {
            case BIRD_STATE_READY:
                // background_tick( &bg );
                // pipes_reset( &pipes );
                break;
            case BIRD_STATE_PLAY:
                // background_tick( &bg );
                // pipes_tick( &pipes );
                // collision_tick( &bird, &pipes );
                break;
        }

        /* Buffer sound effects */
        audio_tick( audio );

        /* Grab a display buffer and start drawing */
        graphics_display_lock( graphics );
        {
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

            /* Draw the pipes */
            // draw_pipes( graphics, pipes );

            /* Draw the bird */
            draw_bird( graphics, bird );

            /* Draw the UI */
            // u16 score = bird.score;
            switch (bird.state)
            {
                case BIRD_STATE_READY:
                    // draw_get_ready( graphics, get_ready );
                    // draw_score( graphics, score );
                    // draw_how_to( graphics, how_to );
                    break;
                case BIRD_STATE_PLAY:
                    // draw_score( graphics, font_large, score );
                    break;
                case BIRD_STATE_DEAD:
                    // draw_death_flash( graphics );
                    // draw_game_over( graphics, game_over );
                    // draw_scoreboard( graphics, scoreboard );
                    // draw_medal( graphics, medals, score );
                    // draw_scoreboard_scores( graphics, font_med, score );
                    break;
            }
        }
        graphics_display_flip( graphics );
    }
}
