/**
 * FlappyBird-N64 - system.h
 *
 * Copyright 2017-2022, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE.txt file in the root directory of this source tree.
 */

#ifndef __FLAPPY_SYSTEM_H
#define __FLAPPY_SYSTEM_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <libdragon.h>

typedef uint32_t color32_t;
typedef int64_t ticks_t;

#define TICKS_PER_MS ((ticks_t)(TICKS_PER_SECOND / 1000))

static inline ticks_t get_total_ms(void)
{
    return (timer_ticks() / TICKS_PER_MS);
}

#define CONTROLLER_1 0

typedef struct SI_condat gamepad_state_t;
typedef struct controller_data controllers_state_t;

bool is_rumble_present(void);

#endif
