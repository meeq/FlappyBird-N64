/**
 * @file joypad.h
 * @author Christopher Bonhage (me@christopherbonhage.com)
 * @brief Joypad Subsystem
 * @ingroup joypad
 */

#ifndef __JOYPAD_H
#define __JOYPAD_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup joypad
 * @{
 */

/** @brief Joypad Port Numbers */
typedef enum
{
    /** @brief Joypad Port 1 */
    JOYPAD_PORT_1     = 0,
    /** @brief Joypad Port 2 */
    JOYPAD_PORT_2     = 1,
    /** @brief Joypad Port 3 */
    JOYPAD_PORT_3     = 2,
    /** @brief Joypad Port 4 */
    JOYPAD_PORT_4     = 3,
    /** @brief Joypad Port Count */
    JOYPAD_PORT_COUNT = 4,
} joypad_port_t;

/** @brief Joypad Style Types */
typedef enum
{
    /** @brief Unplugged or Unknown Controller Style */
    JOYPAD_STYLE_NONE = 0,
    /** @brief Nintendo 64 Controller Style */
    JOYPAD_STYLE_N64  = 1,
    /** @brief GameCube Controller Style */
    JOYPAD_STYLE_GCN  = 2,
} joypad_style_t;

/** @brief Common Joypad Inputs State Structure */
typedef struct joypad_inputs_s
{
    /** @brief State of the A button */
    unsigned a : 1;
    /** @brief State of the B button */
    unsigned b : 1;
    /** @brief State of the Z button */
    unsigned z : 1;
    /** @brief State of the Start button */
    unsigned start : 1;
    /** @brief State of the D-Pad Up button */
    unsigned d_up : 1;
    /** @brief State of the D-Pad Down button */
    unsigned d_down : 1;
    /** @brief State of the D-Pad Left button */
    unsigned d_left : 1;
    /** @brief State of the D-Pad Right button */
    unsigned d_right : 1;
    /**
     * @brief State of the Y button.
     * This input only exists on GCN controllers.
     */
    unsigned y : 1;
    /**
     * @brief State of the X button.
     * This input only exists on GCN controllers.
     */
    unsigned x : 1;
    /** @brief State of the digital L trigger */
    unsigned l : 1;
    /** @brief State of the digital R trigger */
    unsigned r : 1;
    /**
     * @brief State of the C-Up button.
     * For GameCube controllers, the value will be emulated
     * based on the C-Stick Y axis position.
     */
    unsigned c_up : 1;
    /**
     * @brief State of the C-Down button.
     * For GameCube controllers, the value will be emulated
     * based on the C-Stick Y axis position.
     */
    unsigned c_down : 1;
    /**
     * @brief State of the C-Left button.
     * For GameCube controllers, the value will be emulated
     * based on the C-Stick X axis position.
     */
    unsigned c_left : 1;
    /**
     * @brief State of the C-Right button.
     * For GameCube controllers, the value will be emulated
     * based on the C-Stick X axis position.
     */
    unsigned c_right : 1;
    /**
     * @brief State of the joystick X axis.
     * For GCN controllers, the value will be converted from
     * [0,255] to [-128,127] for consistency with the regular
     * N64 controller joystick value.
     */
    signed stick_x : 8;
    /**
     * @brief State of the joystick Y axis.
     * For GCN controllers, the value will be converted from
     * [0,255] to [-128,127] for consistency with the regular
     * N64 controller joystick value.
     */
    signed stick_y : 8;
    /**
     * @brief State of the "C-Stick" X axis.
     * This input only exists on GCN controllers.
     * The value will be converted from [0,255] to [-128,127]
     * for consistency with the joystick value.
     */
    signed cstick_x: 8;
    /**
     * @brief State of the "C-Stick" Y axis.
     * This input only exists on GCN controllers.
     * The value will be converted from [0,255] to [-128,127]
     * for consistency with the joystick value.
     */
    signed cstick_y: 8;
    /**
     * @brief State of the analog L trigger.
     * This value will be close to zero when no pressure is applied,
     * and close to 255 when full pressure is applied.
     * For N64 controllers, this value will be emulated based on the
     * digital L trigger button value (0=unpressed, 255=pressed).
     */
    unsigned analog_l : 8;
    /**
     * @brief State of the analog R trigger.
     * This value will be close to zero when no pressure is applied,
     * and close to 255 when full pressure is applied.
     * For N64 controllers, this value will be emulated based on the
     * digital R trigger button value (0=unpressed, 255=pressed).
     */
    unsigned analog_r : 8;
} joypad_inputs_t;

void joypad_init( void );
void joypad_scan( void );

joypad_style_t joypad_style( joypad_port_t port );
bool joypad_is_rumble_supported( joypad_port_t port );
bool joypad_get_rumble_active( joypad_port_t port );
void joypad_set_rumble_active( joypad_port_t port, bool enabled );

joypad_inputs_t joypad_inputs( joypad_port_t port );
joypad_inputs_t joypad_pressed( joypad_port_t port );
joypad_inputs_t joypad_released( joypad_port_t port );
joypad_inputs_t joypad_held( joypad_port_t port );

#ifdef __cplusplus
}
#endif

/** @} */ /* joypad */

#endif
