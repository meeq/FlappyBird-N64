#ifndef __FLAPPY_SYSTEM_H
#define __FLAPPY_SYSTEM_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#define TICKS_PER_MS 46875 /* (COUNTS_PER_SECOND / 1000) */
#define TICKS_PER_OVERFLOW 91625 /* (ULONG_MAX / TICKS_PER_MS) */
#define DFS_ROM_LOCATION 0xB0041000 /* 256K */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;
typedef signed int s32;

typedef enum boolean
{
    FALSE,
    TRUE
} bool;

typedef struct SI_condat gamepad_state_t;
typedef struct controller_data controllers_state_t;

volatile u32 get_total_ms(void);

#endif
