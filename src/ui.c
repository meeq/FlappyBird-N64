#include "ui.h"

#include "global.h"
#include "background.h"

ui_t ui_setup(void)
{
    ui_t ui = {
        .high_score = 0
    };
    char *sprite_files[UI_NUM_SPRITES] = {
        "/gfx/logo.sprite",
        "/gfx/headings.sprite",
        "/gfx/how-to.sprite",
        "/gfx/scoreboard.sprite",
        "/gfx/medal.sprite",
        "/gfx/font-large.sprite",
        "/gfx/font-medium.sprite"
    };
    for (u8 i = 0; i < UI_NUM_SPRITES; i++)
    {
        ui.sprites[i] = read_dfs_sprite( sprite_files[i] );
    }
    return ui;
}

void ui_free(ui_t *ui)
{
    for (u8 i = 0; i < UI_NUM_SPRITES; i++)
    {
        free( ui->sprites[i] );
        ui->sprites[i] = NULL;
    }
}

inline static void ui_logo_draw(const ui_t ui, u8 time_mode)
{
    sprite_t *logo = ui.sprites[UI_LOGO];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int center_x = (g_graphics->width >> 1);
    int center_y = (g_graphics->height >> 1);
    int logo_x = center_x - (logo->width >> 1);
    int logo_y = center_y - (logo->height * 3.5);

    graphics_draw_sprite_trans( disp, logo_x, logo_y, logo );

    char *credit1_str = "Game by .GEARS";
    int credit1_w = 110;
    int credit1_x = center_x - (credit1_w >> 1);
    int credit1_y = g_graphics->height - 92;

    char *credit2_str = "N64 Port by Bonhage";
    int credit2_w = 150;
    int credit2_x = center_x - (credit2_w >> 1);
    int credit2_y = g_graphics->height - 78;

    u32 fg_color, bg_color;
    if ( time_mode == BG_DAY_TIME )
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

inline static void ui_heading_draw(const ui_t ui, u8 stride)
{
    sprite_t *headings = ui.sprites[UI_HEADINGS];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int center_x = (g_graphics->width >> 1);
    int center_y = (g_graphics->height >> 1);
    int x = center_x - (headings->width >> 1);
    int y = center_y - 70;

    graphics_draw_sprite_trans_stride( disp, x, y, headings, stride );
}

inline static void ui_howto_draw(const ui_t ui)
{
    sprite_t *howto = ui.sprites[UI_HOWTO];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int center_x = (g_graphics->width >> 1);
    int center_y = (g_graphics->height >> 1);
    int x = center_x - (howto->width >> 1);
    int y = center_y - (howto->height / 1.45);

    graphics_draw_sprite_trans( disp, x, y, howto );
}

inline static void ui_score_draw(const ui_t ui, u16 score)
{
    sprite_t *font = ui.sprites[UI_FONT_LARGE];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int i = 0, num_digits;
    int digits[5];
    do
    {
        digits[i] = score % 10;
        score /= 10;
        num_digits = ++i;
    }
    while (score != 0);

    int digit_w = font->width / font->hslices;
    int score_w = digit_w * num_digits;
    int center_x = g_graphics->width >> 1;
    int x = center_x + (score_w >> 1) - digit_w;
    int y = 20;
    for (i = 0; i < num_digits; i++)
    {
        graphics_draw_sprite_trans_stride( disp, x, y, font, digits[i] );
        x -= digit_w;
    }
}

inline static void ui_scoreboard_draw(const ui_t ui)
{
    sprite_t *scoreboard = ui.sprites[UI_SCOREBOARD];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int center_x = (g_graphics->width >> 1);
    int center_y = (g_graphics->height >> 1);
    int x = center_x - (scoreboard->width >> 1);
    int y = center_y - (scoreboard->height >> 1);

    graphics_draw_sprite_trans( disp, x, y, scoreboard );
}

inline static void ui_medal_draw(const ui_t ui, u16 score)
{
    u8 stride;
    if (score >= UI_MEDAL_SCORE_PLATINUM) stride = UI_MEDAL_STRIDE_PLATINUM;
    else if (score >= UI_MEDAL_SCORE_GOLD) stride = UI_MEDAL_STRIDE_GOLD;
    else if (score >= UI_MEDAL_SCORE_SILVER) stride = UI_MEDAL_STRIDE_SILVER;
    else if (score >= UI_MEDAL_SCORE_BRONZE) stride = UI_MEDAL_STRIDE_BRONZE;
    else return;

    sprite_t *medal = ui.sprites[UI_MEDAL];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int center_x = (g_graphics->width >> 1);
    int center_y = (g_graphics->height >> 1);
    int x = center_x - ((medal->width / medal->hslices) >> 1) - 32;
    int y = center_y - ((medal->height / medal->vslices) >> 1) + 4;

    graphics_draw_sprite_trans_stride( disp, x, y, medal, stride );
}

inline static void ui_highscores_score_draw(const ui_t ui, u16 score, u16 y)
{
    sprite_t *font = ui.sprites[UI_FONT_MED];
    int disp = g_graphics->disp;

    int i = 0, num_digits;
    int digits[5];
    do
    {
        digits[i] = score % 10;
        score /= 10;
        num_digits = ++i;
    }
    while (score != 0);

    int digit_w = font->width / font->hslices;
    int center_x = g_graphics->width >> 1;
    int x = center_x + 38;
    for (i = 0; i < num_digits; i++)
    {
        graphics_draw_sprite_trans_stride( disp, x, y, font, digits[i] );
        x -= digit_w;
    }
}

inline static void ui_highscores_draw(const ui_t ui, u16 score)
{
    graphics_detach_rdp( g_graphics );

    int center_y = (g_graphics->height >> 1);
    ui_highscores_score_draw( ui, score, center_y - 11 );
    ui_highscores_score_draw( ui, ui.high_score, center_y + 10);
}

void ui_draw(const ui_t ui, const bird_t bird, const background_t bg)
{
    switch (bird.state)
    {
        case BIRD_STATE_TITLE:
            ui_logo_draw( ui, bg.time_mode );
            break;
        case BIRD_STATE_READY:
            ui_score_draw( ui, bird.score );
            ui_heading_draw( ui, HEADING_GET_READY );
            ui_howto_draw( ui );
            break;
        case BIRD_STATE_PLAY:
        case BIRD_STATE_DYING:
            ui_score_draw( ui, bird.score );
            break;
        case BIRD_STATE_DEAD:
            ui_heading_draw( ui, HEADING_GAME_OVER );
            ui_scoreboard_draw( ui );
            ui_medal_draw( ui, bird.score );
            ui_highscores_draw( ui, bird.score );
            break;
    }
}
