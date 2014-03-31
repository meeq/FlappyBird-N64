#include "pipes.h"

#include "background.h"

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

void pipes_free(pipes_t pipes)
{
    free(pipes.cap_sprite);
    free(pipes.tube_sprite);
}

void pipes_reset(pipes_t *pipes)
{
    if (pipes->reset_ms == pipes->scroll_ms) return;
    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe_t pipe = {
            .x = 1.0 * (i + 1),
            .y = 0.0
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
        for (int i = 0; i < PIPES_MAX_NUM; i++)
        {
            pipes->n[i].x += PIPES_SCROLL_DX;
        }
        pipes->scroll_ms = ticks_ms;
    }
}

/*
    SIGH: The tubes and caps were supposed to be drawn in the same loop
    by leveraging texture slots, but that does not appear to be working.
    Instead we load the tube texture. draw all the tubes, load the cap
    texture, and draw all the caps, which duplicates some computations.
*/

inline static void pipe_draw_tubes(graphics_t *graphics, pipe_t pipe)
{
    if (pipe.x < 0.0 || pipe.x > 1.0) return;
    int cx = graphics->width * pipe.x;
    int tx = cx - (PIPE_WIDTH >> 1);
    int bx = cx + (PIPE_WIDTH >> 1) - 1;
    int cy = (GROUND_TOP_Y >> 1);
    int gap_y = cy + pipe.y * cy;
    int ty, by;
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
}

inline static void pipe_draw_caps(graphics_t *graphics, pipe_t pipe)
{
    if (pipe.x < 0.0 || pipe.x > 1.0) return;
    int cx = graphics->width * pipe.x;
    int tx = cx - (PIPE_WIDTH >> 1);
    int bx = cx + (PIPE_WIDTH >> 1) - 1;
    int cy = (GROUND_TOP_Y >> 1);
    int gap_y = cy + pipe.y * cy;
    int ty, by;
    /* Top cap */
    ty = gap_y - (PIPE_GAP_Y >> 1);
    by = ty + PIPE_CAP_HEIGHT - 1;
    rdp_draw_textured_rectangle_scaled( 0,
        tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
    /* Bottom cap */
    ty = gap_y + (PIPE_GAP_Y >> 1) - PIPE_CAP_HEIGHT;
    by = ty + PIPE_CAP_HEIGHT - 1;
    rdp_draw_textured_rectangle_scaled( 0,
        tx, ty, bx, by, GRAPHICS_SCALE, GRAPHICS_SCALE );
}

void pipes_draw(graphics_t *graphics, pipes_t pipes)
{
    graphics_rdp_texture_fill( graphics );
    mirror_t mirror = MIRROR_DISABLED;
    rdp_sync( SYNC_PIPE );

    rdp_load_texture_stride( 0, 0, mirror, pipes.tube_sprite, 0 );
    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe_draw_tubes( graphics, pipes.n[i] );
    }

    rdp_load_texture_stride( 0, 0, mirror, pipes.cap_sprite, 0 );
    for (int i = 0; i < PIPES_MAX_NUM; i++)
    {
        pipe_draw_caps( graphics, pipes.n[i] );
    }

}
