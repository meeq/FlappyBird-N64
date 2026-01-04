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
#include "fps.h"
#include "pipes.h"
#include "ui.h"

static joypad_buttons_t game_get_buttons_pressed(joypad_port_t port)
{
    joypad_buttons_t buttons = joypad_get_buttons_pressed(port);
    // Treat analog joystick axes as D-pad presses
    const int stick_x = joypad_get_axis_pressed(port, JOYPAD_AXIS_STICK_X);
    const int stick_y = joypad_get_axis_pressed(port, JOYPAD_AXIS_STICK_Y);
    if (stick_x < 0) buttons.d_left = 1;
    if (stick_x > 0) buttons.d_right = 1;
    if (stick_y > 0) buttons.d_up = 1;
    if (stick_y < 0) buttons.d_down = 1;
    return buttons;
}

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

    // rdpq_debug_start();
    // rdpq_debug_log(true);

    /* Initialize game state */
    bg_init();
    fps_init();
    bird_t *const bird = bird_init(BIRD_COLOR_YELLOW);
    pipes_t *const pipes = pipes_init();
    ui_t *const ui = ui_init();
    joypad_buttons_t buttons;

    /* Run the main loop */
    while (1)
    {
        /* Update joypad state */
        joypad_poll();
        buttons = game_get_buttons_pressed(JOYPAD_PORT_1);

        /* Toggle high-res mode with Z button */
        if (buttons.z)
        {
            gfx_set_highres(!gfx_get_highres());
        }

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
            ui_menu_tick(ui, bird, &buttons);
            bg_tick(&buttons);
            break;
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
        fps_tick(&buttons);

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
            bg_draw_sky();
            pipes_draw(pipes);
            bird_draw(bird);
            bg_draw_ground();
            ui_draw(ui);
            fps_draw();
        }
        /* Finish drawing and show the framebuffer */
        rdpq_detach_show();
    }
}
