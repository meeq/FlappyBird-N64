#ifndef __FLAPPY_TITLE_H
#define __FLAPPY_TITLE_H

#include "system.h"
#include "graphics.h"

#define DARK_COLOR graphics_make_color( 0x57, 0x37, 0x47, 0xFF )
#define LIGHT_COLOR graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )
#define CLEAR_COLOR graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

#define HEADING_GET_READY 0
#define HEADING_GAME_OVER 1

void logo_draw(graphics_t *graphics, sprite_t *logo, u8 time_mode);
void heading_draw(graphics_t *graphics, sprite_t *headings, u8 stride);
void howto_draw(graphics_t *graphics, sprite_t *sprite);
void score_draw(graphics_t *graphics, sprite_t *font, u16 score);

#endif
