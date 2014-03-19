#ifndef __FLAPPY_BIRD_H
#define __FLAPPY_BIRD_H

#include "system.h"
#include "graphics.h"

/* Bird definitions */

#define BIRD_STATE_READY 0
#define BIRD_STATE_PLAY 1
#define BIRD_STATE_DEAD 2
#define BIRD_NUM_COLORS 3
#define BIRD_COLOR_YELLOW 0
#define BIRD_COLOR_RED 1
#define BIRD_COLOR_BLUE 2
#define BIRD_ANIM_RATE 150
#define BIRD_ANIM_FRAMES 3
#define BIRD_MIN_Y -0.95
#define BIRD_MAX_Y 0.95
#define BIRD_MIN_ROT -30.0
#define BIRD_MAX_ROT 90.0
#define BIRD_FLAP_VELOCITY 5.0
#define BIRD_WOBBLE_RATE 25
#define BIRD_WOBBLE_INCREMENT 0.1
#define BIRD_WOBBLE_CYCLE (M_PI * 2.0)
#define BIRD_WOBBLE_DAMPEN 0.03

typedef struct
{
    u8 state;
    u8 color_type;
    /* Animation */
    u64 anim_ms;
    u8 anim_frame;
    /* Center point */
    float y;
    /* Rotation */
    float rot;
    /* Flapping */
    float flap_dy;
    u64 flap_ms;
    /* Ready "floating" wave */
    u64 sine_ms;
    float sine_x;
    float sine_y;
} bird_t;

/* Bird helpers */

bird_t bird_setup(u8 color_type);

void draw_bird(graphics_t *graphics, bird_t bird);

void bird_tick(bird_t *bird, gamepad_state_t gamepad);

#endif
