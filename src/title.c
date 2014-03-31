#include "title.h"

#define TEXT_COLOR graphics_make_color( 0x57, 0x37, 0x47, 0xFF )
#define SHADOW_COLOR graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )
#define CLEAR_COLOR graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

void draw_logo(graphics_t *graphics, sprite_t *logo)
{
    graphics_detach_rdp( graphics );
    int disp = graphics->disp;

    int center_x = (graphics->width / 2.0);
    int center_y = (graphics->height / 2.0);
    int logo_x = center_x - (logo->width / 2.0);
    int logo_y = center_y - (logo->height * 3.5);

    graphics_draw_sprite_trans( disp, logo_x, logo_y, logo );

    char *credit1_str = "Game by .GEARS";
    int credit1_w = 110;
    int credit1_x = center_x - (credit1_w / 2.0);
    int credit1_y = graphics->height - 92;
    char *credit2_str = "N64 Port by C. Bonhage";
    int credit2_w = 175;
    int credit2_x = center_x - (credit2_w / 2.0);
    int credit2_y = graphics->height - 78;

    /* Draw a shadow under the text */
    graphics_set_color( SHADOW_COLOR, CLEAR_COLOR );
    graphics_draw_text( disp, credit1_x--, credit1_y--, credit1_str );
    graphics_draw_text( disp, credit2_x--, credit2_y--, credit2_str );

    /* Draw the same text on top of the shadow */
    graphics_set_color( TEXT_COLOR, CLEAR_COLOR );
    graphics_draw_text( disp, credit1_x, credit1_y, credit1_str );
    graphics_draw_text( disp, credit2_x, credit2_y, credit2_str );
}
