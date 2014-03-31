#ifndef __FLAPPY_TITLE_H
#define __FLAPPY_TITLE_H

#include "system.h"
#include "graphics.h"

#define TEXT_COLOR graphics_make_color( 0x57, 0x37, 0x47, 0xFF )
#define SHADOW_COLOR graphics_make_color( 0xFF, 0xFF, 0xFF, 0xFF )
#define CLEAR_COLOR graphics_make_color( 0x00, 0x00, 0x00, 0x00 )

#define HEADING_GET_READY 0
#define HEADING_GAME_OVER 1

void logo_draw(graphics_t *graphics, sprite_t *logo);
void heading_draw(graphics_t *graphics, sprite_t *headings, u8 stride);
void howto_draw(graphics_t *graphics, sprite_t *sprite);

#endif
