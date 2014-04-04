#include "pipes.h"

#include "background.h"
#include "global.h"

pipes_t pipes_setup(void)
{
    pipes_t pipes = {
        .color = PIPE_GREEN_COLOR,
        .reset_ms = 0, .scroll_ms = get_ticks_ms(),
        .cap_sprite = read_dfs_sprite( "/gfx/pipe-cap.sprite" ),
        .tube_sprite = read_dfs_sprite( "/gfx/pipe-tube.sprite" )
    };
    return pipes;
}

void pipes_free(pipes_t *pipes)
{
    free(pipes->cap_sprite);
    pipes->cap_sprite = NULL;
    free(pipes->tube_sprite);
    pipes->tube_sprite = NULL;
}

void pipes_reset(pipes_t *pipes)
{
    if (pipes->reset_ms == pipes->scroll_ms) return;
    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe_t pipe = {
            .x = PIPE_START_X + (i * PIPE_GAP_X),
            .y = 0.0 /* TODO "Randomize" Y */
        };
        pipes->n[i] = pipe;
    }
    pipes->reset_ms = pipes->scroll_ms = get_ticks_ms();
}

void pipes_tick(pipes_t *pipes)
{
    u64 ticks_ms = get_ticks_ms();
    if (ticks_ms - pipes->scroll_ms >= PIPES_SCROLL_RATE)
    {
        for (int i = 0, j; i < PIPES_MAX_NUM; i++)
        {
            pipes->n[i].x += PIPES_SCROLL_DX;
            if (pipes->n[i].x < PIPE_MIN_X)
            {
                j = (i > 0) ? i - 1 : PIPES_MAX_NUM - 1;
                pipes->n[i].x = pipes->n[j].x + PIPE_GAP_X;
            }
        }
        pipes->scroll_ms = ticks_ms;
    }
}

void pipes_draw(const pipes_t pipes)
{
    sprite_t *tube = pipes.tube_sprite;
    sprite_t *cap = pipes.cap_sprite;
    u8 color = pipes.color, cap_hslices = cap->hslices;
    pipe_t pipe;
    int cx, cy, tx, ty, bx, by, gap_y;

    graphics_rdp_texture_fill( g_graphics );
    mirror_t mirror = MIRROR_DISABLED;
    rdp_sync( SYNC_PIPE );
    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe = pipes.n[i];
        /* Calculate X position */
        cx = g_graphics->width * pipe.x;
        tx = cx - (PIPE_TUBE_WIDTH >> 1);
        bx = cx + (PIPE_TUBE_WIDTH >> 1) - 1;
        if (bx < 0 || tx >= g_graphics->width) continue;
        /* Calculate Y position */
        cy = (GROUND_TOP_Y >> 1);
        gap_y = cy + pipe.y * cy;
        /* Load tube texture */
        rdp_load_texture_stride( 0, 0, mirror, tube, color );
        /* Top tube */
        ty = 0;
        by = gap_y - (PIPE_GAP_Y >> 1);
        rdp_draw_textured_rectangle_scaled( 0,
            tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
        /* Bottom tube */
        ty = gap_y + (PIPE_GAP_Y >> 1);
        by = GROUND_TOP_Y - 1;
        rdp_draw_textured_rectangle_scaled( 0,
            tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
        /* Load top cap texture */
        rdp_load_texture_stride( 0, 0, mirror, cap, color + cap_hslices );
        /* Top cap */
        ty = gap_y - (PIPE_GAP_Y >> 1);
        by = ty + PIPE_CAP_HEIGHT - 1;
        rdp_draw_textured_rectangle_scaled( 0,
            tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
        /* Load bottom cap texture */
        rdp_load_texture_stride( 0, 0, mirror, cap, color );
        /* Bottom cap */
        ty = gap_y + (PIPE_GAP_Y >> 1) - PIPE_CAP_HEIGHT;
        by = ty + PIPE_CAP_HEIGHT - 1;
        rdp_draw_textured_rectangle_scaled( 0,
            tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
    }

}
