/**
 * FlappyBird-N64 - bird.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>
#include <math.h>

#include "bird.h"

#include "gfx.h"
#include "sfx.h"
#include "background.h"

/* Bird definitions */

/* Timing */
#define BIRD_RESET_DELAY    ((int) 1000)
#define BIRD_RUMBLE_MS      ((int) 500)

/* Animation */
#define BIRD_ANIM_RATE      ((int) 120)
#define BIRD_ANIM_FRAMES    ((int) 3)
#define BIRD_DYING_FRAME    ((int) 3)

/* Center point */
#define BIRD_TITLE_X        ((float) 0.5)
#define BIRD_PLAY_X         ((float) 0.35)
#define BIRD_ACCEL_X        ((float) 0.001)
#define BIRD_MIN_Y          ((float) -0.90)
#define BIRD_MAX_Y          ((float) 0.95)

/* Flap */
#define BIRD_VELOCITY_RATE  ((int) 16)
#define BIRD_FLAP_VELOCITY  ((float) 0.0270)
#define BIRD_GRAVITY_ACCEL  ((float) 0.0013)

/* Sine "floating" effect */
#define BIRD_SINE_RATE      ((int) 20)
#define BIRD_SINE_INCREMENT ((float) 0.1)
#define BIRD_SINE_CYCLE     ((float) (M_PI * 2.0))
#define BIRD_SINE_DAMPEN    ((float) 0.02)

/* Bird implementation */

bird_t *bird_init(bird_color_t color_type)
{
    sprite_t *const sprite = read_dfs_sprite("gfx/bird.sprite");
    bird_t *const bird = malloc(sizeof(bird_t));
    bird->sprite = sprite;
    bird->slice_w = sprite->width / sprite->hslices;
    bird->slice_h = sprite->height / sprite->vslices;
    bird->state = BIRD_STATE_TITLE;
    bird->color_type = color_type;
    bird->score = 0;
    bird->hit_ms = 0;
    bird->dead_ms = 0;
    bird->is_rumbling = false;
    bird->played_die_sfx = false;
    bird->anim_ms = 0;
    bird->anim_frame = 0;
    bird->x = BIRD_TITLE_X;
    bird->y = 0.0;
    bird->dx = 0.0;
    bird->dy = 0.0;
    bird->dy_ms = 0;
    bird->sine_ms = 0;
    bird->sine_x = 0.0;
    bird->sine_y = 0.0;
    return bird;
}

void bird_free(bird_t *bird)
{
    free(bird->sprite);
    bird->sprite = NULL;
    free(bird);
}

void bird_draw(const bird_t *bird)
{
    /* Calculate player space center position */
    const int cx = gfx->width * bird->x;
    const int cy = BG_GROUND_TOP_Y / 2;
    const mirror_t mirror = MIRROR_DISABLED;
    /* Calculate bird Y position */
    float bird_y = bird->y;
    switch (bird->state)
    {
    case BIRD_STATE_READY:
    case BIRD_STATE_TITLE:
        bird_y += bird->sine_y;
        break;
    default:
        break;
    }
    if (bird_y > BIRD_MAX_Y) bird_y = BIRD_MAX_Y;
    if (bird_y < BIRD_MIN_Y) bird_y = BIRD_MIN_Y;
    bird_y = cy + bird_y * cy;
    /* Calculate bird corner coordinates from center point */
    const int bird_half_w = bird->slice_w / 2,
              bird_half_h = bird->slice_h / 2;
    const int tx = cx - bird_half_w, bx = cx + bird_half_w - 1,
              ty = bird_y - bird_half_h, by = bird_y + bird_half_h - 1;
    /* Load the current animation sprite slice as a texture */
    gfx_rdp_texture_fill();
    rdp_sync(SYNC_PIPE);
    int stride = (bird->color_type * bird->sprite->hslices) + bird->anim_frame;
    rdp_load_texture_stride(0, 0, mirror, bird->sprite, stride);
    /* Draw the bird rectangle */
    rdp_draw_textured_rectangle(0, tx, ty, bx, by, mirror);
}

void bird_hit(bird_t *bird)
{
    bird->hit_ms = get_total_ms();
    sfx_play(SFX_HIT);
    if (is_rumble_present())
    {
        rumble_start(CONTROLLER_1);
        bird->is_rumbling = true;
    }
}

inline static void bird_tick_animation(bird_t *bird)
{
    const ticks_t ticks_ms = get_total_ms();
    ticks_t anim_ms = bird->anim_ms;
    int anim_frame = bird->anim_frame;
    if (bird->state == BIRD_STATE_DYING || bird->state == BIRD_STATE_DEAD)
    {
        /* Dead birds don't animate */
        anim_ms = ticks_ms;
        if (bird->dy < 0.0)
        {
            anim_frame = BIRD_ANIM_FRAMES - 1;
        }
        else
        {
            anim_frame = BIRD_DYING_FRAME;
        }
    }
    else
    {
        if (ticks_ms - anim_ms >= BIRD_ANIM_RATE)
        {
            /* Update animation state */
            if (++anim_frame >= BIRD_ANIM_FRAMES)
            {
                anim_frame = 0;
            }
            anim_ms = ticks_ms;
        }
    }
    bird->anim_ms = anim_ms;
    bird->anim_frame = anim_frame;
}

inline static void bird_tick_dx(bird_t *bird)
{
    /* Move the bird over if needed */
    if (bird->state != BIRD_STATE_TITLE && bird->x > BIRD_PLAY_X)
    {
        bird->dx += BIRD_ACCEL_X;
        bird->x -= bird->dx;
        if (bird->x < BIRD_PLAY_X)
        {
            bird->x = BIRD_PLAY_X;
        }
    }
}

inline static void bird_tick_sine_wave(bird_t *bird)
{
    /* Center the bird in the sky */
    bird->y = 0.0;
    /* Periodically update the "floating" effect */
    const ticks_t ticks_ms = get_total_ms();
    if (ticks_ms - bird->sine_ms >= BIRD_SINE_RATE)
    {
        bird_tick_dx(bird);
        /* Increment the "floating" effect sine wave */
        bird->sine_ms = ticks_ms;
        bird->sine_x += BIRD_SINE_INCREMENT;
        bird->sine_y = sinf(bird->sine_x) * BIRD_SINE_DAMPEN;
        while (bird->sine_x >= BIRD_SINE_CYCLE)
        {
            bird->sine_x -= BIRD_SINE_CYCLE;
        }
    }
}

static void bird_tick_velocity(bird_t *bird, const gamepad_state_t *const gamepad)
{
    /* Flap when the player presses A */
    if (bird->state == BIRD_STATE_PLAY && gamepad->A)
    {
        bird->dy = -BIRD_FLAP_VELOCITY;
        bird->anim_frame = BIRD_ANIM_FRAMES - 1;
        sfx_play(SFX_WING);
    }
    const ticks_t ticks_ms = get_total_ms();
    if (ticks_ms - bird->dy_ms >= BIRD_VELOCITY_RATE)
    {
        bird_tick_dx(bird);
        float y = bird->y;
        float dy = bird->dy;
        dy += BIRD_GRAVITY_ACCEL;
        y += dy;
        /* Did the bird hit the ceiling? */
        if (y < BIRD_MIN_Y)
        {
            y = BIRD_MIN_Y;
        }
        /* Did the bird hit the ground? */
        if (y > BIRD_MAX_Y)
        {
            y = BIRD_MAX_Y;
            dy = 0.0;
            if (bird->state != BIRD_STATE_DYING)
            {
                bird_hit(bird);
            }
            bird->dead_ms = ticks_ms;
            bird->state = BIRD_STATE_DEAD;
        }
        bird->y = y;
        bird->dy = dy;
        bird->dy_ms = ticks_ms;
    }
}

inline static bird_color_t bird_random_color_type(void)
{
    return ((float)rand() / (float)RAND_MAX) * BIRD_COLORS_COUNT;
}

void bird_tick(bird_t *bird, const gamepad_state_t *const gamepad)
{
    const ticks_t ticks_ms = get_total_ms();
    /* State transitions based on button input */
    switch (bird->state)
    {
    case BIRD_STATE_TITLE:
    case BIRD_STATE_DEAD:
        if ((gamepad->A || gamepad->start) &&
            (ticks_ms - bird->dead_ms > BIRD_RESET_DELAY))
        {
            /* Change the bird color after each death */
            if (bird->state == BIRD_STATE_DEAD)
            {
                bird->color_type = bird_random_color_type();
            }
            bird->state = BIRD_STATE_READY;
            bird->score = 0;
            bird->anim_frame = 0;
            bird->played_die_sfx = false;
            sfx_play(SFX_SWOOSH);
        }
        break;
    case BIRD_STATE_READY:
        if (gamepad->A)
        {
            bird->state = BIRD_STATE_PLAY;
        }
        break;
    case BIRD_STATE_DYING:
        if (bird->dy > 0 && !bird->played_die_sfx)
        {
            bird->played_die_sfx = true;
            sfx_play(SFX_DIE);
        }
        break;
    default:
        break;
    }
    /* Cycle through bird colors with right trigger */
    if (gamepad->R)
    {
        if (++bird->color_type >= BIRD_COLORS_COUNT)
        {
            bird->color_type = 0;
        }
    }
    /* State-specific tick updates */
    switch (bird->state)
    {
    case BIRD_STATE_READY:
    case BIRD_STATE_TITLE:
        bird_tick_sine_wave(bird);
        break;
    case BIRD_STATE_PLAY:
    case BIRD_STATE_DYING:
        bird_tick_velocity(bird, gamepad);
        break;
    default:
        break;
    }
    /* Stop rumbling after hitting a pipe/the ground */
    if (bird->is_rumbling && ticks_ms - bird->hit_ms >= BIRD_RUMBLE_MS)
    {
        rumble_stop(CONTROLLER_1);
        bird->is_rumbling = false;
    }
    /* Progress the flapping/falling animation */
    bird_tick_animation(bird);
}
