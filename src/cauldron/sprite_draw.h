#pragma once

#include "sokol_gfx.h"
#include "tx_math.h"
#include "tx_types.h"

typedef struct sprite_handle {
    uint32_t value;
} sprite_handle;

void spr_init(void);
void spr_term(void);
void spr_render();

typedef enum sprite_flip {
    SPRITE_FLIP_NONE = 0,
    SPRITE_FLIP_X = 1,
    SPRITE_FLIP_Y = 2,
} sprite_flip;

typedef struct sprite_draw_desc {
    uint32_t sprite_id;
    float layer;
    vec2 pos;
    vec2 origin;
    sprite_flip flip;
} sprite_draw_desc;

void spr_draw(const sprite_draw_desc* desc);
