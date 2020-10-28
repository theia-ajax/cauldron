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
void actor_system_config_ui(void);
void actor_system_debug_ui(void);

typedef struct actor_system_conf {
    // how much time after the actor has input a platform-drop input do we keep platform-drop mode
    // on
    float platform_drop_time;

    // how much time after the actor becomes ungrounded is the actor still allowed to jump as if
    // they are grounded.
    float jump_ungrounded_time;
} actor_system_conf;

// public system structures

enum actor_flags {
    ActorFlags_FacingLeft = 1 << 0,
    ActorFlags_OnGround = 1 << 1,
    ActorFlags_OnWall = 1 << 2,
    ActorFlags_OnCeiling = 1 << 3,
};

typedef struct actor {
    vec2 pos;
    vec2 vel;
    vec2 hsize;
    uint32_t flags;
    uint32_t sprite_id;
    float platform_timer;
    float jump_forgive_timer;
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

// public system interface

actor_handle actor_create(actor_desc* desc);
void actor_destroy(actor_handle handle);
actor* actor_get(actor_handle handle);