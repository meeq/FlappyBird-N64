/**
 * FlappyBird-N64 - system.c
 *
 * Copyright 2017, Christopher Bonhage
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "system.h"

bool is_rumble_present(void)
{
    const int controllers = get_controllers_present();
    if ( controllers & CONTROLLER_1_INSERTED )
    {
        const int accessories = get_accessories_present( NULL );
        if ( accessories & CONTROLLER_1_INSERTED )
        {
            return identify_accessory( CONTROLLER_1 ) == ACCESSORY_RUMBLEPAK;
        }
    }
    return false;
}
