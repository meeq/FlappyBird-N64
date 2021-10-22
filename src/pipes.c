/**
 * FlappyBird-N64 - pipes.c
 *
 * Copyright 2021, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <stdlib.h>
#include <math.h>

#include "pipes.h"

#include "system.h"
#include "gfx.h"
#include "background.h"

/* Pipes definitions */

#define PIPES_SCROLL_RATE   ((int) 16)
#define PIPES_SCROLL_DX     ((double) -0.00312)

#define PIPE_TUBE_WIDTH     ((int) 26)
#define PIPE_CAP_HEIGHT     ((int) 13)
#define PIPE_GAP_Y          ((int) 80)
#define PIPE_GAP_X          ((float) 0.3)
#define PIPE_START_X        ((float) 1.1)
#define PIPE_MIN_X          ((float) -0.1)
#define PIPE_MAX_Y          ((float) 0.5)
#define PIPE_MAX_BIAS_Y     ((float) 0.4)

/* Pipes implementation */

pipes_t * pipes_init(void)
{
    pipes_t * const pipes = malloc( sizeof(pipes_t) );
    pipes->color = PIPE_COLOR_GREEN;
    pipes->scroll_ms = 0;
    pipes->cap_sprite = read_dfs_sprite( "/gfx/pipe-cap.sprite" );
    pipes->tube_sprite = read_dfs_sprite( "/gfx/pipe-tube.sprite" );
    pipes_reset( pipes );
    return pipes;
}

void pipes_free(pipes_t * pipes)
{
    free( pipes->cap_sprite );
    pipes->cap_sprite = NULL;
    free( pipes->tube_sprite );
    pipes->tube_sprite = NULL;
    free(pipes);
}

inline static pipe_colors_t pipes_random_color(void)
{
    return ((float) rand() / (float) RAND_MAX) * PIPE_NUM_COLORS;
}

inline static size_t pipe_prev_index(size_t current_index)
{
    return (current_index > 0) ? current_index - 1 : PIPES_MAX_NUM - 1;
}

inline static float pipe_random_y(void)
{
    float y = ((float) rand() / (float) RAND_MAX) * PIPE_MAX_Y;
    if ( roundf( (float) rand() / (float) RAND_MAX ) ) y = -y;
    return y;
}

inline static float pipe_random_bias_y(float prev_y)
{
    float bias_y = ((float) rand() / (float) RAND_MAX) * PIPE_MAX_BIAS_Y;
    if ( roundf( (float) rand() / (float) RAND_MAX ) ) bias_y = -bias_y;
    float y = prev_y + bias_y;
    /* If the pipe will be outside the limit, reverse the bias */
    if ( y > PIPE_MAX_Y || y < -PIPE_MAX_Y ) y = prev_y - bias_y;
    return y;
}

void pipes_reset(pipes_t * pipes)
{
    pipe_t * pipe;
    float y = pipe_random_y();
    for (size_t i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe = &pipes->n[i];
        pipe->x = PIPE_START_X + (i * PIPE_GAP_X);
        pipe->y = y;
        pipe->has_scored = false;
        /* Pipes are positioned relative to the previous pipe */
        y = pipe_random_bias_y( y );
    }
    pipes->color = pipes_random_color();
    pipes->scroll_ms = 0;
}

void pipes_tick(pipes_t * pipes)
{
    const ticks_t ticks_ms = get_total_ms();
    /* Start scrolling after a reset */
    if ( pipes->scroll_ms == 0 )
    {
        pipes->scroll_ms = ticks_ms;
    }
    /* Scroll the pipes and reset them as they go off-screen */
    if ( ticks_ms - pipes->scroll_ms >= PIPES_SCROLL_RATE )
    {
        pipe_t * pipe;
        for (size_t i = 0, j; i < PIPES_MAX_NUM; i++)
        {
            pipe = &pipes->n[i];
            pipe->x += PIPES_SCROLL_DX;
            /* Has the pipe gone off the left of the screen? */
            if (pipe->x < PIPE_MIN_X)
            {
                j = pipe_prev_index( i );
                pipe->x = pipes->n[j].x + PIPE_GAP_X;
                pipe->y = pipe_random_bias_y( pipes->n[j].y );
                pipe->has_scored = false;
            }
        }
        pipes->scroll_ms = ticks_ms;
    }
}

void pipes_draw(const pipes_t * pipes)
{
    sprite_t * const tube = pipes->tube_sprite;
    sprite_t * const cap = pipes->cap_sprite;
    const int color = pipes->color;
    const int cap_hslices = cap->hslices;
    const mirror_t mirror = MIRROR_DISABLED;
    int16_t cx, cy, tx, ty, bx, by, gap_cy;

    gfx_rdp_texture_fill();
    rdp_sync( SYNC_PIPE );

    const pipe_t * pipe;
    for (size_t i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe = &pipes->n[i];
        /* Calculate X position */
        cx = gfx->width * pipe->x;
        tx = cx - (PIPE_TUBE_WIDTH >> 1);
        bx = cx + (PIPE_TUBE_WIDTH >> 1) - 1;
        /* Don't bother drawing the pipe if it is off-screen */
        if ( bx < 0 || tx >= gfx->width ) continue;
        /* Calculate Y position */
        cy = (BG_GROUND_TOP_Y >> 1);
        gap_cy = cy + pipe->y * cy;
        /* Load tube texture */
        rdp_load_texture_stride( 0, 0, mirror, tube, color );
        /* Top tube */
        ty = 0;
        by = gap_cy - (PIPE_GAP_Y >> 1);
        rdp_draw_textured_rectangle( 0, tx, ty, bx, by, mirror );
        /* Bottom tube */
        ty = gap_cy + (PIPE_GAP_Y >> 1);
        by = BG_GROUND_TOP_Y - 1;
        rdp_draw_textured_rectangle( 0, tx, ty, bx, by, mirror );
        /* Load top cap texture */
        rdp_load_texture_stride( 0, 0, mirror, cap, color + cap_hslices );
        /* Top cap */
        ty = gap_cy - (PIPE_GAP_Y >> 1);
        by = ty + PIPE_CAP_HEIGHT - 1;
        rdp_draw_textured_rectangle( 0, tx, ty, bx, by, mirror );
        /* Load bottom cap texture */
        rdp_load_texture_stride( 0, 0, mirror, cap, color );
        /* Bottom cap */
        ty = gap_cy + (PIPE_GAP_Y >> 1) - PIPE_CAP_HEIGHT;
        by = ty + PIPE_CAP_HEIGHT - 1;
        rdp_draw_textured_rectangle( 0, tx, ty, bx, by, mirror );
    }
}
