#ifndef __FLAPPY_UI_H
#define __FLAPPY_UI_H

#include "system.h"
#include "graphics.h"

#include "bird.h"
#include "background.h"

#define DARK_COLOR  graphics_make_color( 0x57, 0x37, 0x47, 0xFF )
#define LIGHT_COLOR graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )
#define CLEAR_COLOR graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

#define HEADING_GET_READY   0
#define HEADING_GAME_OVER   1

#define UI_LOGO             0
#define UI_HEADINGS         1
#define UI_HOWTO            2
#define UI_SCOREBOARD       3
#define UI_MEDAL            4
#define UI_FONT_LARGE       5
#define UI_FONT_MED         6
#define UI_NUM_SPRITES      7

#define UI_MEDAL_SCORE_BRONZE       10
#define UI_MEDAL_SCORE_SILVER       20
#define UI_MEDAL_SCORE_GOLD         30
#define UI_MEDAL_SCORE_PLATINUM     40

#define UI_MEDAL_STRIDE_BRONZE      0
#define UI_MEDAL_STRIDE_SILVER      1
#define UI_MEDAL_STRIDE_GOLD        2
#define UI_MEDAL_STRIDE_PLATINUM    3

typedef struct
{
    u16 high_score;
    sprite_t *sprites[UI_NUM_SPRITES];
} ui_t;

ui_t ui_setup(void);

void ui_free(ui_t *ui);

void ui_draw(const ui_t ui, const bird_t bird, const background_t bg);

#endif
