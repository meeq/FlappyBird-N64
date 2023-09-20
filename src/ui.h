/**
 * FlappyBird-N64 - ui.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_UI_H
#define __FLAPPY_UI_H

/* Opaque pointer types */

typedef struct bird_s bird_t;

/* UI declarations */

typedef struct ui_s ui_t;

/* UI functions */

ui_t *ui_init(void);

void ui_free(ui_t *ui);

void ui_tick(ui_t *ui, const bird_t *bird);

void ui_draw(const ui_t *ui);

#endif
