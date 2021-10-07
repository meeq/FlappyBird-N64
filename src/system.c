/**
 * FlappyBird-N64 - system.c
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "system.h"

/* This is adapted from libdragon n64sys.c with overflow support */
volatile u32 get_total_ms(void)
{
    /* Fetch click ticks since startup */
    u32 count;
    // reg $9 on COP0 is count
    asm("\tmfc0 %0,$9\n\tnop":"=r"(count));
    /* Calculate overflow */
    static bool started_count = false;
    static u32 last_count = 0;
    static u32 overflow_ticks = 0;
    if ( started_count && count < last_count )
    {
        overflow_ticks += TICKS_PER_OVERFLOW;
    }
    started_count = true;
    last_count = count;
    /* Calculate total ticks */
    return (count / TICKS_PER_MS) + overflow_ticks;
}

bool is_rumble_present(void)
{
    int controllers = get_controllers_present();
    if ( controllers & CONTROLLER_1_INSERTED )
    {
        int accessories = get_accessories_present( NULL );
        if ( accessories & CONTROLLER_1_INSERTED )
        {
            return identify_accessory( CONTROLLER_1 ) == ACCESSORY_RUMBLEPAK;
        }
    }
    return false;
}
