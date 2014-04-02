#include "collision.h"

void collision_tick(bird_t *bird, pipes_t *pipes, u16 *score)
{
    pipe_t pipe;
    float pipe_x, pipe_y;
    float bird_x = bird->x;

    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe = pipes->n[i];
        pipe_x = pipe.x;
        pipe_y = pipe.y;

        if (bird_x > pipe_x - COLLISION_SCORE_X_TOLERANCE &&
            bird_x < pipe_x + COLLISION_SCORE_X_TOLERANCE)
        {
            *score += 1;
            audio_play_sfx( g_audio, SFX_POINT );
        }
    }
}
