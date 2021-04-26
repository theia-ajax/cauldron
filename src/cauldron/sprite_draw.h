#pragma once

#include "flecs.h"
#include "sokol_gfx.h"
#include "tx_math.h"
#include "tx_types.h"

#include "common_game_components.h"

typedef struct sprite_handle {
    uint32_t value;
} sprite_handle;

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

typedef struct SpriteDraw {
    uint32_t sprite_id;
    sprite_flip flip;
    vec2 origin;
    float layer;
} SpriteDraw;

typedef struct SystemSpriteRenderer {
    ECS_DECLARE_COMPONENT(SpriteDraw);
} SystemSpriteRenderer;

void SystemSpriteRendererImport(ecs_world_t* world);

#define SystemSpriteRendererImportHandles(handles) ECS_IMPORT_COMPONENT(handles, SpriteDraw);