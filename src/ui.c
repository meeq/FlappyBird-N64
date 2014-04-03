#include "ui.h"

#include "background.h"

void ui_logo_draw(graphics_t *graphics, sprite_t *logo, u8 time_mode)
{
    graphics_detach_rdp( graphics );
    int disp = graphics->disp;

    int center_x = (graphics->width >> 1);
    int center_y = (graphics->height >> 1);
    int logo_x = center_x - (logo->width >> 1);
    int logo_y = center_y - (logo->height * 3.5);

    graphics_draw_sprite_trans( disp, logo_x, logo_y, logo );

    char *credit1_str = "Game by .GEARS";
    int credit1_w = 110;
    int credit1_x = center_x - (credit1_w >> 1);
    int credit1_y = graphics->height - 92;

    char *credit2_str = "N64 Port by Bonhage";
    int credit2_w = 150;
    int credit2_x = center_x - (credit2_w >> 1);
    int credit2_y = graphics->height - 78;

    u32 fg_color, bg_color;
    if (time_mode == DAY_TIME)
    {
        fg_color = DARK_COLOR;
        bg_color = LIGHT_COLOR;
    }
    else
    {
        fg_color = LIGHT_COLOR;
        bg_color = DARK_COLOR;
    }

    /* Draw a shadow under the text */
    graphics_set_color( bg_color, CLEAR_COLOR );
    graphics_draw_text( disp, credit1_x--, credit1_y--, credit1_str );
    graphics_draw_text( disp, credit2_x--, credit2_y--, credit2_str );

    /* Draw the same text on top of the shadow */
    graphics_set_color( fg_color, CLEAR_COLOR );
    graphics_draw_text( disp, credit1_x, credit1_y, credit1_str );
    graphics_draw_text( disp, credit2_x, credit2_y, credit2_str );
}

void ui_heading_draw(graphics_t *graphics, sprite_t *headings, u8 stride)
{
    graphics_detach_rdp( graphics );
    int disp = graphics->disp;

    int center_x = (graphics->width >> 1);
    int center_y = (graphics->height >> 1);
    int x = center_x - (headings->width >> 1);
    int y = center_y - 70;

    graphics_draw_sprite_trans_stride( disp, x, y, headings, stride );
}

void ui_howto_draw(graphics_t *graphics, sprite_t *sprite)
{
    graphics_detach_rdp( graphics );
    int disp = graphics->disp;

    int center_x = (graphics->width >> 1);
    int center_y = (graphics->height >> 1);
    int x = center_x - (sprite->width >> 1);
    int y = center_y - (sprite->height / 1.45);

    graphics_draw_sprite_trans( disp, x, y, sprite );
}

void ui_score_draw(graphics_t *graphics, sprite_t *sprite, u16 score)
{
    graphics_detach_rdp( graphics );
    int disp = graphics->disp;

    int i = 0, num_digits;
    int digits[5];
    do
    {
        digits[i] = score % 10;
        score /= 10;
        num_digits = ++i;
    }
    while (score != 0);

    int digit_w = sprite->width / sprite->hslices;
    int score_w = digit_w * num_digits;
    int center_x = graphics->width >> 1;
    int x = center_x + (score_w >> 1) - digit_w;
    int y = 20;
    for (i = 0; i < num_digits; i++)
    {
        graphics_draw_sprite_trans_stride( disp, x, y, sprite, digits[i] );
        x -= digit_w;
    }
}
