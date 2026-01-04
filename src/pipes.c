/**
 * FlappyBird-N64 - pipes.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>
#include <math.h>

#include "pipes.h"

#include "system.h"
#include "gfx.h"
#include "bg.h"

/* Pipes definitions */

#define PIPES_SCROLL_RATE   ((int)16 * TICKS_PER_MS)
#define PIPES_SCROLL_DX     ((float)-0.00312)
#define PIPE_TUBE_WIDTH     ((int)26)
#define PIPE_CAP_HEIGHT     ((int)13)
#define PIPE_GAP_Y          ((int)80)
#define PIPE_GAP_X          ((float)0.3)
#define PIPE_START_X        ((float)1.1)
#define PIPE_MIN_X          ((float)-0.1)
#define PIPE_MAX_Y          ((float)0.5)
#define PIPE_MAX_BIAS_Y     ((float)0.4)

/* Pipes implementation */

pipes_t *pipes_init(void)
{
    pipes_t *const pipes = malloc(sizeof(pipes_t));
    pipes->color = PIPE_COLOR_GREEN;
    pipes->scroll_ticks = 0;
    pipes->cap_sprite = read_dfs_sprite("gfx/pipe-cap.sprite");
    pipes->tube_sprite = read_dfs_sprite("gfx/pipe-tube.sprite");
    pipes_reset(pipes);
    return pipes;
}

void pipes_free(pipes_t *pipes)
{
    free(pipes->cap_sprite);
    pipes->cap_sprite = NULL;
    free(pipes->tube_sprite);
    pipes->tube_sprite = NULL;
    free(pipes);
}

static pipe_color_t pipes_random_color(void)
{
    return ((float)rand() / (float)RAND_MAX) * PIPE_COLORS_COUNT;
}

static size_t pipe_prev_index(size_t current_index)
{
    return (current_index > 0) ? current_index - 1 : PIPES_MAX_COUNT - 1;
}

static float pipe_random_y(void)
{
    float y = ((float)rand() / (float)RAND_MAX) * PIPE_MAX_Y;
    if (roundf((float)rand() / (float)RAND_MAX))
        y = -y;
    return y;
}

static float pipe_random_bias_y(float prev_y)
{
    float bias_y = ((float)rand() / (float)RAND_MAX) * PIPE_MAX_BIAS_Y;
    if (roundf((float)rand() / (float)RAND_MAX))
        bias_y = -bias_y;
    float y = prev_y + bias_y;
    /* If the pipe will be outside the limit, reverse the bias */
    if (y > PIPE_MAX_Y || y < -PIPE_MAX_Y)
        y = prev_y - bias_y;
    return y;
}

void pipes_reset(pipes_t *pipes)
{
    pipe_t *pipe;
    float y = pipe_random_y();
    for (size_t i = 0; i < PIPES_MAX_COUNT; i++)
    {
        pipe = &pipes->n[i];
        pipe->x = PIPE_START_X + (i * PIPE_GAP_X);
        pipe->y = y;
        pipe->has_scored = false;
        /* Pipes are positioned relative to the previous pipe */
        y = pipe_random_bias_y(y);
    }
    pipes->color = pipes_random_color();
    pipes->scroll_ticks = 0;
}

void pipes_tick(pipes_t *pipes)
{
    const uint64_t now_ticks = get_ticks();
    /* Start scrolling after a reset */
    if (pipes->scroll_ticks == 0)
    {
        pipes->scroll_ticks = now_ticks;
    }
    /* Scroll the pipes and reset them as they go off-screen */
    if ((now_ticks - pipes->scroll_ticks) >= PIPES_SCROLL_RATE)
    {
        pipe_t *pipe;
        for (size_t i = 0, j; i < PIPES_MAX_COUNT; i++)
        {
            pipe = &pipes->n[i];
            pipe->x += PIPES_SCROLL_DX;
            /* Has the pipe gone off the left of the screen? */
            if (pipe->x < PIPE_MIN_X)
            {
                j = pipe_prev_index(i);
                pipe->x = pipes->n[j].x + PIPE_GAP_X;
                pipe->y = pipe_random_bias_y(pipes->n[j].y);
                pipe->has_scored = false;
            }
        }
        pipes->scroll_ticks = now_ticks;
    }
}

void pipes_draw(const pipes_t *pipes)
{
    sprite_t *const tube = pipes->tube_sprite;
    sprite_t *const cap = pipes->cap_sprite;
    const int color = pipes->color;
    int16_t cx, cy, tx, ty, bx, by, gap_cy;

    /* Calculate sprite slice dimensions */
    const int tube_slice_w = tube->width / tube->hslices;
    const int cap_slice_w = cap->width / cap->hslices;
    const int cap_slice_h = cap->height / cap->vslices;

    /* Texture offset for the pipe color */
    const int tube_s_offset = color * tube_slice_w;
    const int cap_s_offset = color * cap_slice_w;
    /* Top cap uses the second row (flipped cap) */
    const int top_cap_t_offset = cap_slice_h;

    rdpq_set_mode_copy(true);

    const pipe_t *pipe;
    for (size_t i = 0; i < PIPES_MAX_COUNT; i++)
    {
        pipe = &pipes->n[i];
        /* Calculate X position */
        cx = gfx->width * pipe->x;
        tx = cx - (PIPE_TUBE_WIDTH / 2);
        bx = cx + (PIPE_TUBE_WIDTH / 2);
        /* Don't bother drawing the pipe if it is off-screen */
        if (bx < 0 || tx >= gfx->width) continue;
        /* Calculate Y position */
        cy = (BG_GROUND_TOP_Y / 2);
        gap_cy = cy + pipe->y * cy;

        /* Draw tube sections with tiling */
        rdpq_texparms_t tube_texparams = {
            .s = { .repeats = 1, .mirror = MIRROR_NONE },
            .t = { .repeats = REPEAT_INFINITE, .mirror = MIRROR_NONE },
        };
        rdpq_sprite_upload(TILE0, tube, &tube_texparams);

        /* Top tube */
        ty = 0;
        by = gap_cy - (PIPE_GAP_Y / 2);
        rdpq_texture_rectangle(TILE0, tx, ty, bx, by, tube_s_offset, 0);

        /* Bottom tube */
        ty = gap_cy + (PIPE_GAP_Y / 2);
        by = BG_GROUND_TOP_Y;
        rdpq_texture_rectangle(TILE0, tx, ty, bx, by, tube_s_offset, 0);

        /* Top cap (uses flipped sprite in second row) */
        ty = gap_cy - (PIPE_GAP_Y / 2);
        rdpq_sprite_blit(cap, tx, ty, &(rdpq_blitparms_t){
            .s0 = cap_s_offset,
            .t0 = top_cap_t_offset,
            .width = cap_slice_w,
            .height = cap_slice_h,
        });

        /* Bottom cap */
        ty = gap_cy + (PIPE_GAP_Y / 2) - PIPE_CAP_HEIGHT;
        rdpq_sprite_blit(cap, tx, ty, &(rdpq_blitparms_t){
            .s0 = cap_s_offset,
            .width = cap_slice_w,
            .height = cap_slice_h,
        });
    }
}
