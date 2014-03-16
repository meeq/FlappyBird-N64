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
