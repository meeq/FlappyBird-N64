#include "ui.h"

#include "global.h"
#include "background.h"

inline static void ui_set_time_mode(ui_t *ui, bg_time_mode_t time_mode)
{
    ui->time_mode = time_mode;
    if ( time_mode == BG_DAY_TIME )
    {
        ui->text_color = UI_DARK_COLOR;
        ui->shadow_color = UI_LIGHT_COLOR;
    }
    else
    {
        ui->text_color = UI_LIGHT_COLOR;
        ui->shadow_color = UI_DARK_COLOR;
    }
}

ui_t ui_setup(const background_t bg)
{
    ui_t ui = {
        .last_score = 0,
        .high_score = 0,
        .new_high_score = FALSE,
        .clear_color = UI_CLEAR_COLOR,
        .flash_draw = FALSE,
        .flash_color = UI_FLASH_COLOR,
        .dead_ms = 0,
        .heading_draw = FALSE,
        .board_draw = FALSE,
        .score_draw = FALSE,
        .medal_draw = FALSE,
        .board_y = g_graphics->height,
        .score_ms = 0,
        .last_score_acc = 0,
        .high_score_acc = 0
    };
    ui_set_time_mode( &ui, bg.time_mode );
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

void ui_tick(ui_t *ui, const bird_t bird, const background_t bg)
{
    const u64 ticks_ms = get_ticks_ms();
    /* Synchronize bird state to UI */
    ui->state = bird.state;
    /* High scoring */
    ui->last_score = bird.score;
    if ( bird.score == 0 )
    {
        ui->new_high_score = FALSE;
    }
    if ( bird.score > ui->high_score )
    {
        ui->high_score = bird.score;
        ui->new_high_score = TRUE;
    }
    /* Flash the screen for a split second after the bird dies */
    if ( bird.state == BIRD_STATE_DYING ||
         bird.state == BIRD_STATE_DEAD )
    {
        ui->flash_draw = ticks_ms - bird.die_ms <= UI_DEATH_FLASH_MS;
    }
    /* Animate the Game Over UI */
    if ( bird.state == BIRD_STATE_DEAD )
    {
        ui->dead_ms = bird.dead_ms;
        const u64 dead_diff_ms = ticks_ms - bird.dead_ms;
        /* Only show the scores and medal after the scoreboard appears */
        ui->score_draw = FALSE;
        ui->medal_draw = FALSE;
        /* Show the game over heading and play a sound */
        const bool was_heading_draw = ui->heading_draw;
        ui->heading_draw = dead_diff_ms > UI_DEATH_HEADING_DELAY;
        if ( !was_heading_draw && ui->heading_draw )
        {
            audio_play_sfx( g_audio, SFX_SWOOSH );
        }
        /* Show the scoreboard and play a sound */
        const bool was_board_draw = ui->board_draw;
        ui->board_draw = dead_diff_ms > UI_DEATH_BOARD_DELAY;
        if ( !was_board_draw && ui->board_draw )
        {
            audio_play_sfx( g_audio, SFX_SWOOSH );
        }
        if ( ui->board_draw )
        {
            const u64 board_diff_ms = dead_diff_ms - UI_DEATH_BOARD_DELAY;
            sprite_t *scoreboard = ui->sprites[UI_SPRITE_SCOREBOARD];
            const u16 max_y = g_graphics->height;
            const u16 center_y = max_y >> 1;
            const u16 min_y = center_y - (scoreboard->height >> 1);
            ui->score_draw = board_diff_ms > UI_DEATH_BOARD_DY_MS;
            if ( !ui->score_draw )
            {
                /* Pop the scoreboard up from the bottom */
                const u16 y_diff = max_y - min_y;
                const float y_factor = board_diff_ms / UI_DEATH_BOARD_DY_MS;
                ui->board_y = max_y - (y_diff * y_factor);
                /* Reset the score accumulators */
                ui->score_ms = ticks_ms;
                ui->last_score_acc = 0;
            }
            else
            {
                ui->board_y = min_y;
            }
        }
        if ( ui->score_draw )
        {
            if ( ui->last_score_acc < ui->last_score ||
                 ui->high_score_acc < ui->high_score )
            {
                const u64 score_diff_ms = ticks_ms - ui->score_ms;
                if ( score_diff_ms >= UI_DEATH_SCORE_DELAY )
                {
                    if ( ui->last_score_acc < ui->last_score )
                    {
                        ui->last_score_acc++;
                    }
                    else if ( ui->high_score_acc < ui->high_score )
                    {
                        ui->high_score_acc++;
                    }
                    ui->score_ms = ticks_ms;
                }
            }
            else
            {
                ui->medal_draw = TRUE;
            }
        }
    }
    /* Synchronize background state to UI */
    if ( ui->time_mode != bg.time_mode )
    {
        ui_set_time_mode( ui, bg.time_mode );
    }
}

/* Below this point there be magic numbers! */

inline static void ui_logo_draw(const ui_t ui)
{
    sprite_t *logo = ui.sprites[UI_SPRITE_LOGO];

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

    /* Draw a shadow under the text */
    graphics_set_color( ui.shadow_color, ui.clear_color );
    graphics_draw_text( disp, credit1_x--, credit1_y--, credit1_str );
    graphics_draw_text( disp, credit2_x--, credit2_y--, credit2_str );

    /* Draw the same text on top of the shadow */
    graphics_set_color( ui.text_color, ui.clear_color );
    graphics_draw_text( disp, credit1_x, credit1_y, credit1_str );
    graphics_draw_text( disp, credit2_x, credit2_y, credit2_str );
}

inline static void ui_heading_draw(const ui_t ui, u8 stride)
{
    sprite_t *headings = ui.sprites[UI_SPRITE_HEADINGS];

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
    sprite_t *howto = ui.sprites[UI_SPRITE_HOWTO];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int center_x = (g_graphics->width >> 1);
    int center_y = (g_graphics->height >> 1);
    int x = center_x - (howto->width >> 1);
    int y = center_y - (howto->height / 1.45);

    graphics_draw_sprite_trans( disp, x, y, howto );
}

inline static void ui_score_draw(const ui_t ui)
{
    u16 score = ui.last_score;
    sprite_t *font = ui.sprites[UI_SPRITE_FONT_LARGE];

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
    sprite_t *scoreboard = ui.sprites[UI_SPRITE_SCOREBOARD];

    graphics_detach_rdp( g_graphics );
    int disp = g_graphics->disp;

    int center_x = (g_graphics->width >> 1);
    int x = center_x - (scoreboard->width >> 1);

    graphics_draw_sprite_trans( disp, x, ui.board_y, scoreboard );
}

inline static void ui_medal_draw(const ui_t ui)
{
    u8 stride;
    const u16 score = ui.last_score;
    if (score >= UI_MEDAL_SCORE_PLATINUM) stride = UI_MEDAL_STRIDE_PLATINUM;
    else if (score >= UI_MEDAL_SCORE_GOLD) stride = UI_MEDAL_STRIDE_GOLD;
    else if (score >= UI_MEDAL_SCORE_SILVER) stride = UI_MEDAL_STRIDE_SILVER;
    else if (score >= UI_MEDAL_SCORE_BRONZE) stride = UI_MEDAL_STRIDE_BRONZE;
    else return;

    sprite_t *medal = ui.sprites[UI_SPRITE_MEDAL];

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
    sprite_t *font = ui.sprites[UI_SPRITE_FONT_MED];
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

inline static void ui_highscores_draw(const ui_t ui)
{
    graphics_detach_rdp( g_graphics );

    int center_y = (g_graphics->height >> 1);
    ui_highscores_score_draw( ui, ui.last_score_acc, center_y - 11 );
    ui_highscores_score_draw( ui, ui.high_score_acc, center_y + 10);
}

inline static void ui_flash_draw(const ui_t ui)
{
    graphics_rdp_color_fill( g_graphics );
    rdp_set_primitive_color( ui.flash_color );
    const u16 bx = g_graphics->width - 1;
    const u16 by = g_graphics->height - 1;
    rdp_draw_filled_rectangle( 0, 0, bx, by );
}

void ui_draw(const ui_t ui)
{
    if ( ui.flash_draw )
    {
        ui_flash_draw( ui );
        return;
    }
    switch (ui.state)
    {
        case BIRD_STATE_TITLE:
            ui_logo_draw( ui );
            break;
        case BIRD_STATE_READY:
            ui_score_draw( ui );
            ui_heading_draw( ui, UI_HEADING_GET_READY );
            ui_howto_draw( ui );
            break;
        case BIRD_STATE_PLAY:
        case BIRD_STATE_DYING:
            ui_score_draw( ui );
            break;
        case BIRD_STATE_DEAD:
            if ( ui.heading_draw )
            {
                ui_heading_draw( ui, UI_HEADING_GAME_OVER );
            }
            if ( ui.board_draw )
            {
                ui_scoreboard_draw( ui );
            }
            if ( ui.score_draw )
            {
                ui_highscores_draw( ui );
            }
            if ( ui.medal_draw )
            {
                ui_medal_draw( ui );
            }
            break;
    }
}
