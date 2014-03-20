#include <math.h>

#include "background.h"
#include "bird.h"


static sprite_t *bird_sprite = NULL;
static float bird_half_w = 0.0;
static float bird_half_h = 0.0;
static float bird_half_hyp = 0.0;

bird_t bird_setup(u8 color_type)
{
    if (bird_sprite == NULL)
    {
        /* Load the sprite and pre-calculate some size details */
        bird_sprite = read_dfs_sprite( "/gfx/bird.sprite" );
        /* Calculate the half-hypotenuse of the slice diagonal */
        float slice_w = (bird_sprite->width / bird_sprite->hslices) - 1.0;
        float slice_h = (bird_sprite->height / bird_sprite->vslices) - 1.0;
        float hyp_squared = powf( slice_w, 2.0 ) + powf( slice_h, 2.0 );
        bird_half_w = slice_w / 2.0;
        bird_half_h = slice_h / 2.0;
        bird_half_hyp = sqrtf( hyp_squared ) / 2.0;
    }
    bird_t bird = {
        .state = BIRD_STATE_READY,
        .color_type = color_type,
        .anim_ms = 0,
        .anim_frame = 0,
        .y = 0.0,
        .rot = 0.0,
        .dy = 0.0,
        .flap_ms = 0,
        .gravity_ms = 0,
        .sine_ms = 0,
        .sine_x = 0.0,
        .sine_y = 0.0
    };
    return bird;
}

void draw_bird(graphics_t *graphics, bird_t bird)
{
    /* Calculate player space center position */
    u16 cx = graphics->width / 2.0,
        cy = GROUND_TOP_Y / 2.0;
    /* Calculate bird Y position */
    float bird_y = bird.y;
    if (bird.state == BIRD_STATE_READY)
    {
        bird_y += bird.sine_y;
    }
    if (bird_y > BIRD_MAX_Y) bird_y = BIRD_MAX_Y;
    if (bird_y < BIRD_MIN_Y) bird_y = BIRD_MIN_Y;
    cy += bird_y * cy;
    /* TODO Calculate rotation from center point */
    u16 tx = cx - bird_half_w, bx = cx + bird_half_w,
        ty = cy - bird_half_h, by = cy + bird_half_h;
    /* Setup the RDP for textured fills if it isn't already */
    if (graphics->rdp_fill_mode != RDP_FILL_TEXTURE)
    {
        /* Enable textures instead of solid color fill */
        rdp_enable_texture_copy();
        graphics->rdp_fill_mode = RDP_FILL_TEXTURE;

    }
    /* Load the current animation sprite slice as a texture */
    rdp_sync( SYNC_PIPE );
    u8 stride = (bird.color_type * BIRD_NUM_COLORS) + bird.anim_frame;
    rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, bird_sprite, stride );
    /* Draw the rotated rectangle */
    rdp_draw_textured_rectangle( 0, tx, ty, bx, by );
}

static void bird_tick_animation(bird_t *bird)
{
    u64 ticks_ms = get_ticks_ms(),
        anim_ms = bird->anim_ms;
    u8 anim_frame = bird->anim_frame;
    if (bird->state != BIRD_STATE_DEAD)
    {
        if (ticks_ms - anim_ms > BIRD_ANIM_RATE)
        {
            /* Update animation state */
            if (++anim_frame >= BIRD_ANIM_FRAMES)
            {
                anim_frame = 0;
            }
            anim_ms = ticks_ms;
        }
    } else {
        /* Dead birds don't animate */
        anim_ms = ticks_ms;
        anim_frame = BIRD_ANIM_FRAMES - 1;
    }
    bird->anim_ms = anim_ms;
    bird->anim_frame = anim_frame;
}

static void bird_tick_ready(bird_t *bird)
{
    /* Center the bird in the sky */
    bird->y = 0.0;
    bird->rot = 0.0;
    /* Periodically update the "floating" effect */
    u64 ticks_ms = get_ticks_ms();
    if (ticks_ms - bird->sine_ms > BIRD_SINE_RATE)
    {
        /* Increment the "floating" effect sine wave */
        bird->sine_ms = ticks_ms;
        bird->sine_x += BIRD_SINE_INCREMENT;
        bird->sine_y = sinf( bird->sine_x ) * BIRD_SINE_DAMPEN;
        while (bird->sine_x >= BIRD_SINE_CYCLE)
        {
            bird->sine_x -= BIRD_SINE_CYCLE;
        }
    }
}

static void bird_tick_velocity(bird_t *bird, gamepad_state_t gamepad)
{
    /* Flap when the player presses A */
    if ( gamepad.A )
    {
        bird->dy -= BIRD_FLAP_VELOCITY;
    }
    u64 ticks_ms = get_ticks_ms();
    if (ticks_ms - bird->gravity_ms > BIRD_GRAVITY_RATE)
    {
        float bird_y = bird->y;
        float bird_dy = bird->dy;
        bird_y += BIRD_GRAVITY_VELOCITY + bird_dy;
        /* Did the bird hit the ground? */
        if (bird_y > BIRD_MAX_Y)
        {
            bird_y = BIRD_MAX_Y;
            bird->state = BIRD_STATE_DEAD;
        }
        /* Did the bird hit the ceiling? */
        if (bird_y < BIRD_MIN_Y)
        {
            bird_y = BIRD_MIN_Y;
        }
        if (bird_dy < 0.0)
        {
            bird_dy += BIRD_GRAVITY_VELOCITY;
            if (bird_dy > 0.0) bird_dy = 0.0;
        }
        bird->y = bird_y;
        bird->dy = bird_dy;
        bird->gravity_ms = ticks_ms;
    }
}

void bird_tick(bird_t *bird, gamepad_state_t gamepad)
{
    /* Cycle through bird states with start button */
    if ( gamepad.start )
    {
        if (++bird->state >= BIRD_NUM_STATES)
        {
            bird->state = 0;
        }
    }
    /* Cycle through bird colors with right trigger */
    if ( gamepad.R )
    {
        if (++bird->color_type >= BIRD_NUM_COLORS)
        {
            bird->color_type = 0;
        }
    }
    bird_tick_animation( bird );
    switch (bird->state)
    {
        case BIRD_STATE_READY:
            bird_tick_ready( bird );
            break;
        case BIRD_STATE_PLAY:
            bird_tick_velocity( bird, gamepad );
            // bird_tick_rotation( bird );
            break;
    }
}
