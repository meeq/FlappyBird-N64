#ifndef __FLAPPY_COLLISION_H
#define __FLAPPY_COLLISION_H

#include "system.h"
#include "audio.h"

#include "bird.h"
#include "pipes.h"

#define COLLISION_PIPE_HALF_WIDTH   0.1
#define COLLISION_PIPE_HALF_GAP     0.4
#define COLLISION_SCORE_X_TOLERANCE 0.001

void collision_tick(bird_t *bird, pipes_t *pipes, u16 *score);

#endif
