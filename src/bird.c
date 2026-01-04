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
#include "bg.h"

/* Bird definitions */

/* Timing */
#define BIRD_RESET_DELAY    (1000 * TICKS_PER_MS)
#define BIRD_RUMBLE_MS      (500 * TICKS_PER_MS)

/* Animation */
#define BIRD_ANIM_RATE      (120 * TICKS_PER_MS)
#define BIRD_ANIM_FRAMES    ((int) 3)
#define BIRD_DYING_FRAME    ((int) 3)

/* Center point */
#define BIRD_TITLE_X        ((float) 0.5)
#define BIRD_PLAY_X         ((float) 0.35)
#define BIRD_ACCEL_X        ((float) 0.001)
#define BIRD_MIN_Y          ((float) -0.90)
#define BIRD_MAX_Y          ((float) 0.95)

/* Flap */
#define BIRD_VELOCITY_RATE  (16 * TICKS_PER_MS)
#define BIRD_FLAP_VELOCITY  ((float) 0.0270)
#define BIRD_GRAVITY_ACCEL  ((float) 0.0013)

/* Sine "floating" effect */
#define BIRD_SINE_RATE      (20 * TICKS_PER_MS)
#define BIRD_SINE_INCREMENT ((float) 0.1)
#define BIRD_SINE_CYCLE     ((float) (M_PI * 2.0))
#define BIRD_SINE_DAMPEN    ((float) 0.02)

/* Rotation */
#define BIRD_ROTATION_UP_DEG    ((float) (20.0 * M_PI / 180.0))
#define BIRD_ROTATION_UP_MS     100
#define BIRD_ROTATION_DOWN_DEG  ((float) (-90.0 * M_PI / 180.0))
#define BIRD_ROTATION_DOWN_MS   600
#define BIRD_ROTATION_HOLD_MS   300

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
    bird->hit_ticks = 0;
    bird->dead_ticks = 0;
    bird->is_dead_reset = true;
    bird->is_rumbling = false;
    bird->played_die_sfx = false;
    bird->anim_ticks = 0;
    bird->anim_frame = 0;
    bird->x = BIRD_TITLE_X;
    bird->y = 0.0;
    bird->dx = 0.0;
    bird->dy = 0.0;
    bird->dy_ticks = 0;
    bird->sine_ticks = 0;
    bird->sine_x = 0.0;
    bird->sine_y = 0.0;
    bird->rotation = 0.0;
    bird->flap_ticks = 0;
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
    /* Calculate texture offset for current animation frame and color */
    const int s_offset = bird->anim_frame * bird->slice_w;
    const int t_offset = bird->color_type * bird->slice_h;
    /* Draw the bird sprite with rotation */
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_sprite_blit(bird->sprite, cx, bird_y, &(rdpq_blitparms_t){
        .s0 = s_offset,
        .t0 = t_offset,
        .width = bird->slice_w,
        .height = bird->slice_h,
        .cx = bird->slice_w / 2,
        .cy = bird->slice_h / 2,
        .theta = bird->rotation,
    });
}

void bird_hit(bird_t *bird)
{
    bird->hit_ticks = get_ticks();
    sfx_play(SFX_HIT);
    joypad_set_rumble_active(JOYPAD_PORT_1, bird->is_rumbling = true);
}

static void bird_tick_animation(bird_t *bird)
{
    const uint64_t now_ticks = get_ticks();
    uint64_t anim_ticks = bird->anim_ticks;
    int anim_frame = bird->anim_frame;
    if (bird->state != BIRD_STATE_DYING && bird->state != BIRD_STATE_DEAD)
    {
        if ((now_ticks - anim_ticks) >= BIRD_ANIM_RATE)
        {
            /* Update animation state */
            if (++anim_frame >= BIRD_ANIM_FRAMES)
            {
                anim_frame = 0;
            }
            anim_ticks = now_ticks;
        }
    }
    bird->anim_ticks = anim_ticks;
    bird->anim_frame = anim_frame;
}

static void bird_tick_dx(bird_t *bird)
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

static void bird_tick_sine_wave(bird_t *bird)
{
    /* Center the bird in the sky */
    bird->y = 0.0;
    /* Periodically update the "floating" effect */
    const uint64_t now_ticks = get_ticks();
    if ((now_ticks - bird->sine_ticks) >= BIRD_SINE_RATE)
    {
        bird_tick_dx(bird);
        /* Increment the "floating" effect sine wave */
        bird->sine_ticks = now_ticks;
        bird->sine_x += BIRD_SINE_INCREMENT;
        bird->sine_y = sinf(bird->sine_x) * BIRD_SINE_DAMPEN;
        while (bird->sine_x >= BIRD_SINE_CYCLE)
        {
            bird->sine_x -= BIRD_SINE_CYCLE;
        }
    }
}

static void bird_tick_velocity(bird_t *bird, const joypad_buttons_t *const buttons)
{
    /* Flap when the player presses A */
    if (bird->state == BIRD_STATE_PLAY && buttons->a)
    {
        bird->dy = -BIRD_FLAP_VELOCITY;
        bird->anim_frame = BIRD_ANIM_FRAMES - 1;
        bird->flap_ticks = get_ticks();
        sfx_play(SFX_WING);
    }
    const uint64_t now_ticks = get_ticks();
    if ((now_ticks - bird->dy_ticks) >= BIRD_VELOCITY_RATE)
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
            bird->dead_ticks = now_ticks;
            bird->state = BIRD_STATE_DEAD;
        }
        bird->y = y;
        bird->dy = dy;
        bird->dy_ticks = now_ticks;
    }
}

static void bird_tick_rotation(bird_t *bird)
{
    if (bird->state == BIRD_STATE_TITLE || bird->state == BIRD_STATE_READY)
    {
        bird->rotation = 0.0;
        return;
    }
    if (bird->state == BIRD_STATE_DEAD)
    {
        bird->rotation = BIRD_ROTATION_DOWN_DEG;
        return;
    }

    const uint64_t now_ticks = get_ticks();
    const uint64_t elapsed_ms = (now_ticks - bird->flap_ticks) / TICKS_PER_MS;

    if (elapsed_ms < BIRD_ROTATION_UP_MS && bird->rotation < BIRD_ROTATION_UP_DEG)
    {
        /* Phase 1: Rotating up (skip if already at up position) */
        float t = (float)elapsed_ms / BIRD_ROTATION_UP_MS;
        bird->rotation = t * BIRD_ROTATION_UP_DEG;
    }
    else if (elapsed_ms < BIRD_ROTATION_UP_MS + BIRD_ROTATION_HOLD_MS && bird->state != BIRD_STATE_DYING)
    {
        /* Phase 2: Hold at up position (skip when dying) */
        bird->rotation = BIRD_ROTATION_UP_DEG;
    }
    else
    {
        /* Phase 3: Rotating down */
        uint64_t hold_time = (bird->state == BIRD_STATE_DYING) ? 0 : BIRD_ROTATION_HOLD_MS;
        uint64_t fall_elapsed = elapsed_ms - BIRD_ROTATION_UP_MS - hold_time;
        float t = (float)fall_elapsed / BIRD_ROTATION_DOWN_MS;
        if (t > 1.0f) t = 1.0f;
        bird->rotation = BIRD_ROTATION_UP_DEG + t * (BIRD_ROTATION_DOWN_DEG - BIRD_ROTATION_UP_DEG);
    }
}

static bird_color_t bird_random_color_type(void)
{
    return ((float)rand() / (float)RAND_MAX) * BIRD_COLORS_COUNT;
}

void bird_tick(bird_t *bird, const joypad_buttons_t *const buttons)
{
    const uint64_t now_ticks = get_ticks();
    /* State transitions based on button input */
    switch (bird->state)
    {
    case BIRD_STATE_DEAD:
        if ((now_ticks - bird->dead_ticks) >= BIRD_RESET_DELAY)
        {
            bird->is_dead_reset = true;
        }
    case BIRD_STATE_TITLE:
        if ((buttons->a || buttons->start) && bird->is_dead_reset)
        {
            /* Change the bird color after each death */
            if (bird->state == BIRD_STATE_DEAD)
            {
                bird->color_type = bird_random_color_type();
            }
            bird->state = BIRD_STATE_READY;
            bird->score = 0;
            bird->anim_frame = 0;
            bird->is_dead_reset = false;
            bird->played_die_sfx = false;
            sfx_play(SFX_SWOOSH);
        }
        break;
    case BIRD_STATE_READY:
        if (buttons->a)
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
    if (buttons->r)
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
        bird_tick_velocity(bird, buttons);
        break;
    default:
        break;
    }
    /* Stop rumbling after hitting a pipe/the ground */
    if (bird->is_rumbling && (now_ticks - bird->hit_ticks) >= BIRD_RUMBLE_MS)
    {
        joypad_set_rumble_active(JOYPAD_PORT_1, bird->is_rumbling = false);
    }
    /* Progress the flapping/falling animation */
    bird_tick_animation(bird);
    /* Update rotation based on time since last flap */
    bird_tick_rotation(bird);
}
