#include <math.h>

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
        float hyp_squared = pow( slice_w, 2.0 ) + pow( slice_h, 2.0 );
        bird_half_w = slice_w / 2.0;
        bird_half_h = slice_h / 2.0;
        bird_half_hyp = sqrt( hyp_squared ) / 2.0;
    }
    bird_t bird = {
        .anim_frame = 0,
        .color_type = color_type,
        .y = 0.0,
        .rot = 0.0
    };
    return bird;
}

void bird_tick(bird_t *bird, gamepad_state_t gamepad)
{
    /* Update animation state */
    u64 ticks = get_ticks_ms(), anim_tick = bird->anim_tick;
    u8 anim_frame = bird->anim_frame;
    while (ticks - anim_tick > BIRD_ANIM_RATE)
    {
        anim_frame++;
        if (anim_frame == BIRD_ANIM_FRAMES)
        {
            anim_frame = 0;
        }
        anim_tick = ticks;
    }
    bird->anim_tick = anim_tick;
    bird->anim_frame = anim_frame;
}

void draw_bird(graphics_t *graphics, bird_t bird)
{
    if (graphics->rdp_attached != RDP_ATTACHED) return;
    /* It's probably best not to thrash between textures and colors */
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
    /* Calculate bird rectangle position */
    u16 cx = graphics->width / 2.0, cy = graphics->height / 2.0,
        tx = cx - bird_half_w, bx = cx + bird_half_w,
        ty = cy - bird_half_h, by = cy + bird_half_h;
    /* TODO Calculate Y position */
    /* TODO Calculate rotation */
    rdp_draw_textured_rectangle( 0, tx, ty, bx, by );
}
