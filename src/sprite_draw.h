#pragma once

#include "sokol_gfx.h"
#include "tx_math.h"
#include "tx_types.h"

typedef struct sprite_handle {
    uint32_t value;
} sprite_handle;

void spr_init();
void spr_shutdown();
void spr_render(int width, int height);

typedef struct sprite_draw_desc {
    uint32_t sprite_id;
    float layer;
    vec2 pos;
    vec2 origin;
} sprite_draw_desc;

void spr_draw(const sprite_draw_desc* desc);