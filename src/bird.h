#ifndef __FLAPPY_BIRD_H
#define __FLAPPY_BIRD_H

#include "system.h"
#include "graphics.h"

/* Bird definitions */

/* States */
#define BIRD_STATE_READY    0
#define BIRD_STATE_PLAY     1
#define BIRD_STATE_DYING    2
#define BIRD_STATE_DEAD     3
#define BIRD_STATE_TITLE    4
#define BIRD_DEAD_DELAY     1000
/* Colors */
#define BIRD_NUM_COLORS     3
#define BIRD_COLOR_YELLOW   0
#define BIRD_COLOR_RED      1
#define BIRD_COLOR_BLUE     2
/* Animation */
#define BIRD_ANIM_RATE      120
#define BIRD_ANIM_FRAMES    3
/* Center point */
#define BIRD_TITLE_X        0.5
#define BIRD_PLAY_X         0.35
#define BIRD_ACCEL_X        0.001
#define BIRD_MIN_Y          -0.90
#define BIRD_MAX_Y          0.95
/* Rotation */
#define BIRD_MIN_ROT        -15.0
#define BIRD_MAX_ROT        90.0
/* Flap */
#define BIRD_VELOCITY_RATE 16
#define BIRD_FLAP_VELOCITY 0.05
#define BIRD_GRAVITY_ACCEL 0.0045
/* Sine "floating" effect */
#define BIRD_SINE_RATE      20
#define BIRD_SINE_INCREMENT 0.1
#define BIRD_SINE_CYCLE     (M_PI * 2.0)
#define BIRD_SINE_DAMPEN    0.02

typedef struct
{
    u8 state;
    u8 color_type;
    u64 dead_ms;
    u16 score;
    /* Animation */
    u64 anim_ms;
    u8 anim_frame;
    /* Center point */
    float x;
    float y;
    float dx;
    /* Rotation */
    float rot;
    /* Flapping */
    float dy;
    u64 dy_ms;
    /* Ready "floating" wave */
    u64 sine_ms;
    float sine_x;
    float sine_y;
} bird_t;

/* Bird helpers */

bird_t bird_setup(u8 color_type);

void bird_draw(const bird_t bird);

void bird_tick(bird_t *bird, gamepad_state_t gamepad);

#endif
