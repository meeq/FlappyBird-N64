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

#include "background.h"
#include "bird.h"
#include "collision.h"
#include "fps.h"
#include "pipes.h"
#include "ui.h"

int main(void)
{
    /* Initialize libdragon subsystems */
    timer_init();
    controller_init();
    dfs_init(DFS_DEFAULT_LOCATION);

    /* Initialize game subsystems */
    sfx_init();
    gfx_init(
        RESOLUTION_320x240, DEPTH_16_BPP, BUFFERING_DOUBLE,
        GAMMA_NONE, ANTIALIAS_RESAMPLE_FETCH_ALWAYS);
    fps_init();

    /* Initialize game state */
    background_t *const bg = background_init(BG_TIME_DAY);
    bird_t *const bird = bird_init(BIRD_COLOR_YELLOW);
    pipes_t *const pipes = pipes_init();
    ui_t *const ui = ui_init(bg);

    /* Run the main loop */
    while (1)
    {
        /* Update controller state */
        controller_scan();
        const controllers_state_t controllers = get_keys_down();
        const gamepad_state_t *const gamepad = &controllers.c[CONTROLLER_1];

        /* Calculate frame timing */
        fps_tick(gamepad);

        /* Update bird state before the rest of the world */
        const bird_state_t prev_bird_state = bird->state;
        bird_tick(bird, gamepad);

        /* Reset the world when the bird resets after dying */
        if (prev_bird_state != bird->state && prev_bird_state == BIRD_STATE_DEAD)
        {
            background_randomize_time_mode(bg);
            pipes_reset(pipes);
        }

        /* Update the world state based on the bird state */
        switch (bird->state)
        {
        case BIRD_STATE_TITLE:
        case BIRD_STATE_READY:
            background_tick(bg, gamepad);
            break;
        case BIRD_STATE_PLAY:
            background_tick(bg, gamepad);
            pipes_tick(pipes);
            collision_tick(bird, pipes);
            break;
        default:
            break;
        }

        /* Update the UI based on the world state */
        ui_tick(ui, bird, bg);

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
            background_draw(bg);
            pipes_draw(pipes);
            bird_draw(bird);
            ui_draw(ui);
            fps_draw();
        }
        /* Finish drawing and show the framebuffer */
        gfx_display_flip();
    }
}
