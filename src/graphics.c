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

void graphics_free(graphics_t *graphics)
{
    free( graphics );
    rdp_close();
    display_close();
}


void graphics_display_lock(graphics_t *graphics)
{
    /* Grab a render buffer */
    static display_context_t disp = 0;
    while( !(disp = display_lock()) );
    graphics->disp = disp;
    /* Reset RDP state */
    graphics->rdp_attached = RDP_DETACHED;
    graphics->rdp_fill_mode = RDP_FILL_NONE;
}

void graphics_display_flip(graphics_t *graphics)
{
    if( graphics->disp )
    {
        /* Detach the RDP and sync before flipping the display buffer */
        if (graphics->rdp_attached == RDP_ATTACHED)
        {
            graphics_detach_rdp(graphics);
        }
        /* Force backbuffer flip and reset the display handle */
        display_show( graphics->disp );
        graphics->disp = 0;
    }
}

void graphics_attach_rdp(graphics_t *graphics)
{
    if (graphics->rdp_attached == RDP_DETACHED && graphics->disp)
    {
        /* Ensure the RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Attach RDP to display */
        rdp_attach_display( graphics->disp );
        graphics->rdp_attached = RDP_ATTACHED;
    }
}

void graphics_detach_rdp(graphics_t *graphics)
{
    if (graphics->rdp_attached == RDP_ATTACHED)
    {
        /* Inform the RDP drawing is finished; flush pending operations */
        rdp_detach_display();
        graphics->rdp_attached = RDP_DETACHED;
        graphics->rdp_fill_mode = RDP_FILL_NONE;
    }
}

void graphics_rdp_color_fill(graphics_t *graphics)
{
    graphics_attach_rdp(graphics);
    /* Setup the RDP for color fills if it isn't already */
    if (graphics->rdp_fill_mode != RDP_FILL_COLOR)
    {
        /* Enable solid colors instead of texture fills */
        rdp_enable_primitive_fill();
        graphics->rdp_fill_mode = RDP_FILL_COLOR;
    }
}

void graphics_rdp_texture_fill(graphics_t *graphics)
{
    graphics_attach_rdp(graphics);
    /* Setup the RDP for textured fills if it isn't already */
    if (graphics->rdp_fill_mode != RDP_FILL_TEXTURE)
    {
        /* Enable textures instead of solid color fill */
        rdp_enable_texture_copy();
        graphics->rdp_fill_mode = RDP_FILL_TEXTURE;
    }
}

sprite_t *read_dfs_sprite(char *file)
{
    int fp = dfs_open( file );
    sprite_t *sprite = malloc( dfs_size( fp ) );
    dfs_read( sprite, 1, dfs_size( fp ), fp );
    dfs_close( fp );
    return sprite;
}
