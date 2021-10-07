/**
 * FlappyBird-N64 - main.c
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "system.h"
#include "sfx.h"
#include "gfx.h"

#include "ui.h"
#include "background.h"
#include "bird.h"
#include "pipes.h"
#include "collision.h"

#include "fps.h"
#include "global.h"

int main(void)
{
    /* Enable interrupts (on the CPU) */
    init_interrupts();
    timer_init();

    /* Initialize peripherals */
    dfs_init( DFS_DEFAULT_LOCATION );
    controller_init();

    /* Initialize audio */
    audio_init( FREQUENCY_44KHZ, 4 );
    audio_write_silence();
    sfx_init();

    /* Initialize display */
    gfx_init(
        RESOLUTION_320x240, DEPTH_16_BPP, BUFFERING_DOUBLE,
        GAMMA_NONE, ANTIALIAS_RESAMPLE_FETCH_ALWAYS
    );
    fps_counter_t fps = fps_setup();

    /* Initialize game state */
    background_t bg = background_setup( BG_DAY_TIME );
    bird_t bird = bird_setup( BIRD_COLOR_YELLOW );
    pipes_t pipes = pipes_setup();
    ui_t ui = ui_setup( &bg );

    /* Run the main loop */
    while ( true )
    {
        /* Update controller state */
        controller_scan();
        const controllers_state_t controllers = get_keys_down();
        const gamepad_state_t gamepad = controllers.c[0];

        /* Calculate frame timing */
        fps_tick( &fps, &gamepad );

        /* Update bird state before the rest of the world */
        const bird_state_t prev_state = bird.state;
        bird_tick( &bird, &gamepad );

        /* Reset the world when the bird resets after dying */
        if ( prev_state != bird.state && prev_state == BIRD_STATE_DEAD )
        {
            background_randomize_time_mode( &bg );
            pipes_reset( &pipes );
        }

        /* Update the world state based on the bird state */
        switch (bird.state)
        {
            case BIRD_STATE_TITLE:
            case BIRD_STATE_READY:
                background_tick( &bg, &gamepad );
                break;
            case BIRD_STATE_PLAY:
                background_tick( &bg, &gamepad );
                pipes_tick( &pipes );
                collision_tick( &bird, &pipes );
                break;
            default:
                break;
        }

        /* Update the UI based on the world state */
        ui_tick( &ui, &bird, &bg );

        /* Buffer sound effects */
        if ( audio_can_write() )
        {
            short *buf = audio_write_begin();
            mixer_poll(buf, audio_get_buffer_length());
            audio_write_end();
        }

        /* Grab a display buffer and start drawing */
        gfx_display_lock();
        {
            /* Draw the game state */
            background_draw( &bg );
            pipes_draw( &pipes );
            bird_draw( &bird );
            ui_draw( &ui );
            fps_draw( &fps );
        }
        /* Finish drawing and show the framebuffer */
        gfx_display_flip();
    }

    /* Clean up game state */
    ui_free( &ui );
    pipes_free( &pipes );
    bird_free( &bird );
    background_free( &bg );

    /* Clean up the initialized subsystems */
    gfx_close();
    sfx_close();
    audio_close();

    return 0;
}
