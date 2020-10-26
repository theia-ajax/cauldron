#pragma once

#include "game_systems_forward.h"
#include "handle.h"
#include "tx_math.h"
#include "tx_types.h"

// game systems interface

void actor_system_init(game_settings* settings);
void actor_system_shutdown(void);
void actor_system_load_level(game_level* level);
void actor_system_unload_level(void);
void actor_system_update(float dt);
void actor_system_render(void);

// system structures

enum actor_flags {
    ACTOR_FLAGS_FACING_LEFT = 1 << 0,
    ACTOR_FLAGS_GROUNDED = 1 << 1,
};

typedef struct actor {
    vec2 pos;
    vec2 vel;
    vec2 hsize;
    uint32_t flags;
    uint32_t sprite_id;
    struct {
        vec2 move;
        bool jump;
    } input;
} actor;

DEFINE_HANDLE(actor);

typedef struct actor_desc {
    vec2 pos;
    vec2 hsize;
    uint32_t sprite_id;
} actor_desc;

// system interface

actor_handle actor_create(actor_desc* desc);
void actor_destroy(actor_handle handle);
actor* actor_get(actor_handle handle);