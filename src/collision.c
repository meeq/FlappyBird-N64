/**
 * FlappyBird-N64 - collision.c
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#include "collision.h"

#include "sfx.h"
#include "bird.h"
#include "pipes.h"

#define COLLISION_PIPE_HALF_WIDTH   0.052
#define COLLISION_PIPE_HALF_GAP     0.23
#define COLLISION_SCORE_X_TOLERANCE 0.01

void collision_tick(bird_t * bird, pipes_t * pipes)
{
    pipe_t * pipe;
    const float bird_x = bird->x, bird_y = bird->y;
    for (size_t i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe = &pipes->n[i];
        const float pipe_x = pipe->x, pipe_y = pipe->y;
        if ( bird_x > pipe_x - COLLISION_PIPE_HALF_WIDTH &&
             bird_x < pipe_x + COLLISION_PIPE_HALF_WIDTH )
        {
            if ( bird_y > pipe_y + COLLISION_PIPE_HALF_GAP ||
                 bird_y < pipe_y - COLLISION_PIPE_HALF_GAP )
            {
                bird->state = BIRD_STATE_DYING;
                bird_hit( bird );
            }
            else if ( bird_x > pipe_x - COLLISION_SCORE_X_TOLERANCE &&
                      bird_x < pipe_x + COLLISION_SCORE_X_TOLERANCE &&
                      !pipe->has_scored )
            {
                bird->score += 1;
                pipe->has_scored = true;
                sfx_play( SFX_POINT );
            }
        }
    }
}
