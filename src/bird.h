#ifndef __FLAPPY_BIRD_H
#define __FLAPPY_BIRD_H

#include "system.h"
#include "graphics.h"

/* Bird definitions */

#define BIRD_ANIM_RATE 100
#define BIRD_ANIM_FRAMES 3
#define BIRD_NUM_COLORS 3
#define BIRD_COLOR_YELLOW 0
#define BIRD_COLOR_RED 1
#define BIRD_COLOR_BLUE 2

typedef struct
{
    u64 anim_tick;
    u8 anim_frame;
    u8 color_type;
    /* Center point */
    float y;
    /* Rotation */
    float rot;
} bird_t;

/* Bird helpers */

bird_t bird_setup(u8 color_type);

void draw_bird(graphics_t *graphics, bird_t bird);

void bird_tick(bird_t *bird, gamepad_state_t gamepad);

#endif
