#ifndef __FLAPPY_UI_H
#define __FLAPPY_UI_H

#include "system.h"
#include "graphics.h"
#include "global.h"
#include "background.h"

#define DARK_COLOR graphics_make_color( 0x57, 0x37, 0x47, 0xFF )
#define LIGHT_COLOR graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )
#define CLEAR_COLOR graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

#define HEADING_GET_READY 0
#define HEADING_GAME_OVER 1

void ui_logo_draw(sprite_t *logo, u8 time_mode);
void ui_heading_draw(sprite_t *headings, u8 stride);
void ui_howto_draw(sprite_t *sprite);
void ui_score_draw(sprite_t *font, u16 score);

#endif
