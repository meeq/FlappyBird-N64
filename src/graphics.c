#include "graphics.h"


graphics_t *graphics_setup(resolution_t res, bitdepth_t depth,
                           u32 num_buffers, gamma_t gamma,
                           antialias_t aa)
{
    /* Set up the display and RDP subsystems */
    display_init(res, depth, num_buffers, gamma, aa);
    rdp_init();
    /* Figure out the screen resolution */
    u16 width = 0, height = 0;
    switch( res )
    {
        case RESOLUTION_320x240:
            width = 320;
            height = 240;
            break;
        case RESOLUTION_640x480:
            width = 640;
            height = 480;
            break;
        case RESOLUTION_256x240:
            width = 256;
            height = 240;
            break;
        case RESOLUTION_512x480:
            width = 512;
            height = 480;
            break;
    }
    graphics_t *graphics = malloc( sizeof( graphics_t ) );
    /* Setup state */
    graphics->res = res;
    graphics->width = width;
    graphics->height = height;
    graphics->color_depth = depth;
    graphics->num_buffers = num_buffers;
    graphics->gamma = gamma;
    graphics->antialias = aa;
    /* Drawing state */
    graphics->disp = 0;
    /* RDP state */
    graphics->rdp_attached = RDP_DETACHED;
    graphics->rdp_fill_mode = RDP_FILL_NONE;
    return graphics;
}

void free_graphics(graphics_t *graphics)
{
    free( graphics );
    rdp_close();
    display_close();
}

sprite_t *read_dfs_sprite(char *file)
{
    int fp = dfs_open( file );
    sprite_t *sprite = malloc( dfs_size( fp ) );
    dfs_read( sprite, 1, dfs_size( fp ), fp );
    dfs_close( fp );
    return sprite;
}

void draw_x_fill_color(graphics_t *graphics, x_fill_color_t fill)
{
    if (graphics->rdp_attached != RDP_ATTACHED) return;
    /* It's probably best not to thrash between colors and textures */
    if (graphics->rdp_fill_mode != RDP_FILL_COLOR)
    {
        /* Enable solid color fill instead of textures */
        rdp_enable_primitive_fill();
        graphics->rdp_fill_mode = RDP_FILL_COLOR;
    }
    rdp_set_primitive_color( fill.color );
    rdp_draw_filled_rectangle( 0,
        fill.y, graphics->width, fill.y + fill.h );
}

void draw_x_fill_sprite(graphics_t *graphics, x_fill_sprite_t fill)
{
    if (graphics->rdp_attached != RDP_ATTACHED) return;
    /* It's probably best not to thrash between textures and colors */
    if (graphics->rdp_fill_mode != RDP_FILL_TEXTURE)
    {
        /* Enable textures instead of solid color fill */
        rdp_enable_texture_copy();
        graphics->rdp_fill_mode = RDP_FILL_TEXTURE;

    }
    int slices = fill.sprite->hslices, max_w = graphics->width;
    if (slices > 1)
    {
        /* Manually tile horizontally-sliced repeating fills */
        for (int repeat_x = 0,
                 repeat_w = fill.sprite->width / slices,
                 slice;
             repeat_x < max_w; )
        {
            for (slice = 0;
                 slice < slices && repeat_x < max_w;
                 slice++, repeat_x += repeat_w)
            {
                rdp_sync( SYNC_PIPE );
                rdp_load_texture_stride( 0, 0, MIRROR_DISABLED,
                    fill.sprite, slice );
                rdp_draw_sprite( 0, repeat_x, fill.y );
            }
        }
    } else {
        /* Small tiles can be drawn in a single rectangle */
        rdp_sync( SYNC_PIPE );
        rdp_load_texture( 0, 0, MIRROR_DISABLED, fill.sprite );
        rdp_draw_textured_rectangle( 0,
            0, fill.y, max_w, fill.y + fill.sprite->height - 1);
    }
}
