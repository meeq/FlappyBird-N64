/**
 * FlappyBird-N64 - main.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include "system.h"
#include "gfx.h"
#include "sfx.h"

#include "bg.h"
#include "bird.h"
#include "collision.h"
#include "pipes.h"
#include "ui.h"

int main(void)
{
    // Initialize debug logs
    debug_init_isviewer();
    debug_init_usblog();

    /* Initialize libdragon subsystems */
    timer_init();
    joypad_init();
    dfs_init(DFS_DEFAULT_LOCATION);

    /* Initialize game subsystems */
    gfx_init();
    sfx_init();

    /* Initialize game state */
    bg_init();
    bird_t *const bird = bird_init(BIRD_COLOR_YELLOW);
    pipes_t *const pipes = pipes_init();
    ui_t *const ui = ui_init();
    joypad_buttons_t buttons;

    /* Run the main loop */
    while (1)
    {
        /* Update joypad state */
        joypad_poll();
        buttons = joypad_get_buttons(JOYPAD_PORT_1);

        /* Update bird state before the rest of the world */
        const bird_state_t prev_bird_state = bird->state;
        bird_tick(bird, &buttons);

        /* Reset the world when the bird resets after dying */
        if (prev_bird_state != bird->state && prev_bird_state == BIRD_STATE_DEAD)
        {
            bg_randomize_time_mode();
            pipes_reset(pipes);
        }

        /* Update the world state based on the bird state */
        switch (bird->state)
        {
        case BIRD_STATE_TITLE:
        case BIRD_STATE_READY:
            bg_tick(&buttons);
            break;
        case BIRD_STATE_PLAY:
            bg_tick(&buttons);
            pipes_tick(pipes);
            collision_tick(bird, pipes);
            break;
        default:
            break;
        }

        /* Update the UI based on the world state */
        ui_tick(ui, bird);

        /* Buffer sound effects */
        if (audio_can_write())
        {
            short *const buf = audio_write_begin();
            mixer_poll(buf, audio_get_buffer_length());
            audio_write_end();
        }

        /* Grab a display buffer and start drawing */
        gfx_display_lock();
        {
            /* Draw the game state */
            bg_draw();
            pipes_draw(pipes);
            bird_draw(bird);
            // ui_draw(ui);
        }
        /* Finish drawing and show the framebuffer */
        rdpq_detach_show();
    }
}
