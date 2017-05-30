/**
 * FlappyBird-N64 - collision.c
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "collision.h"

#include "audio.h"
#include "global.h"

void collision_tick(bird_t *bird, pipes_t *pipes)
{
    const float bird_x = bird->x, bird_y = bird->y;
    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        const pipe_t pipe = pipes->n[i];
        const float pipe_x = pipe.x, pipe_y = pipe.y;
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
                      !pipe.has_scored )
            {
                bird->score += 1;
                pipes->n[i].has_scored = true;
                audio_play_sfx( g_audio, SFX_POINT );
            }
        }
    }
}
