#include "collision.h"

#include "audio.h"
#include "global.h"

void collision_tick(bird_t *bird, pipes_t *pipes)
{
    pipe_t pipe;
    float pipe_x, pipe_y;
    float bird_x = bird->x, bird_y = bird->y;

    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe = pipes->n[i];
        pipe_x = pipe.x;
        pipe_y = pipe.y;

        if (bird_x > pipe_x - COLLISION_PIPE_HALF_WIDTH &&
            bird_x < pipe_x + COLLISION_PIPE_HALF_WIDTH)
        {
            if (bird_y > pipe_y + COLLISION_PIPE_HALF_GAP ||
                bird_y < pipe_y - COLLISION_PIPE_HALF_GAP)
            {
                bird->state = BIRD_STATE_DYING;
                audio_play_sfx( g_audio, SFX_HIT );
            }
            else if (bird_x > pipe_x - COLLISION_SCORE_X_TOLERANCE &&
                     bird_x < pipe_x + COLLISION_SCORE_X_TOLERANCE)
            {
                bird->score += 1;
                audio_play_sfx( g_audio, SFX_POINT );
            }
        }
    }
}
