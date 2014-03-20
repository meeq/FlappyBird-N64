#ifndef __FLAPPY_BIRD_H
#define __FLAPPY_BIRD_H

#include "system.h"
#include "graphics.h"

/* Bird definitions */

/* States */
#define BIRD_NUM_STATES 3
#define BIRD_STATE_READY 0
#define BIRD_STATE_PLAY 1
#define BIRD_STATE_DEAD 2
/* Colors */
#define BIRD_NUM_COLORS 3
#define BIRD_COLOR_YELLOW 0
#define BIRD_COLOR_RED 1
#define BIRD_COLOR_BLUE 2
/* Animation */
#define BIRD_ANIM_RATE 150
#define BIRD_ANIM_FRAMES 3
/* Center point */
#define BIRD_MIN_Y -0.90
#define BIRD_MAX_Y 0.95
/* Rotation */
#define BIRD_MIN_ROT -30.0
#define BIRD_MAX_ROT 90.0
/* Flap */
#define BIRD_FLAP_VELOCITY 0.08
#define BIRD_GRAVITY_RATE 10
#define BIRD_GRAVITY_VELOCITY 0.01
/* Sine "floating" effect */
#define BIRD_SINE_RATE 25
#define BIRD_SINE_INCREMENT 0.1
#define BIRD_SINE_CYCLE (M_PI * 2.0)
#define BIRD_SINE_DAMPEN 0.03

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
    float dy;
    u64 flap_ms;
    u64 gravity_ms;
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
