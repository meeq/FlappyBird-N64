#ifndef __JOYPAD_H
#define __JOYPAD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    JOYPAD_PORT_1     = 0,
    JOYPAD_PORT_2     = 1,
    JOYPAD_PORT_3     = 2,
    JOYPAD_PORT_4     = 3,
    JOYPAD_PORT_COUNT = 4,
} joypad_port_t;

typedef enum
{
    JOYPAD_STYLE_NONE = 0,
    JOYPAD_STYLE_N64 = 1,
    JOYPAD_STYLE_GCN = 2,
} joypad_style_t;

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
    /** @brief State of the L button */
    unsigned l : 1;
    /** @brief State of the R button */
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
     * [0,255] to [-128,128] for consistency with the regular
     * N64 controller joystick value.
     */
    signed stick_x : 8;
    /**
     * @brief State of the joystick Y axis.
     * For GCN controllers, the value will be converted from
     * [0,255] to [-128,128] for consistency with the regular
     * N64 controller joystick value.
     */
    signed stick_y : 8;
    /**
     * @brief State of the "C-Stick" X axis.
     * This input only exists on GCN controllers.
     * The value will be converted from [0,255] to [-128,128]
     * for consistency with the joystick value.
     */
    signed cstick_x: 8;
    /**
     * @brief State of the "C-Stick" Y axis.
     * This input only exists on GCN controllers.
     * The value will be converted from [0,255] to [-128,128]
     * for consistency with the joystick value.
     */
    signed cstick_y: 8;
    /**
     * @brief State of the analog L trigger.
     * This input only exists on GCN controllers.
     * This value will be close to zero when no pressure is applied,
     * and close to 255 when full pressure is applied.
     */
    unsigned analog_l : 8;
    /**
     * @brief State of the analog R trigger.
     * This input only exists on GCN controllers.
     * This value will be close to zero when no pressure is applied,
     * and close to 255 when full pressure is applied.
     */
    unsigned analog_r : 8;
} joypad_inputs_t;

void joypad_init( void );
void joypad_identify( bool reset );
void joypad_poll( void );

joypad_style_t joypad_style( joypad_port_t port );
bool joypad_is_rumble_supported( joypad_port_t port );
bool joypad_get_rumble_state( joypad_port_t port );
void joypad_set_rumble_state( joypad_port_t port, bool enabled );

joypad_inputs_t joypad_inputs( joypad_port_t port );
joypad_inputs_t joypad_pressed( joypad_port_t port );
joypad_inputs_t joypad_released( joypad_port_t port );
joypad_inputs_t joypad_held( joypad_port_t port );

#endif
