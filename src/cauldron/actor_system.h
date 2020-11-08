#pragma once

#include "game_systems_forward.h"
#include "tx_math.h"
#include "tx_types.h"

// public system structures

enum actor_flags {
    ActorFlags_OnGround = 1 << 0,
    ActorFlags_OnWall = 1 << 1,
    ActorFlags_OnCeiling = 1 << 2,
    ActorFlags_HitGround = 1 << 3,
    ActorFlags_HitWall = 1 << 4,
    ActorFlags_HitCeiling = 1 << 5,
    ActorFlags_LeftGround = 1 << 6,
    ActorFlags_LeftWall = 1 << 7,
    ActorFlags_LeftCeiling = 1 << 8,

    ActorFlags_FacingLeft = 1 << 9,

    ActorFlags_AllMoveResultOnFlags =
        ActorFlags_OnGround | ActorFlags_OnWall | ActorFlags_OnCeiling,
    ActorFlags_AllMoveResultHitFlags =
        ActorFlags_HitGround | ActorFlags_HitWall | ActorFlags_HitCeiling,
    ActorFlags_AllMoveResultLeftFlags =
        ActorFlags_LeftGround | ActorFlags_LeftWall | ActorFlags_LeftCeiling,
    ActorFlags_AllMoveResultFlags = ActorFlags_AllMoveResultOnFlags
                                    | ActorFlags_AllMoveResultHitFlags
                                    | ActorFlags_AllMoveResultLeftFlags,
};

typedef struct actor_def actor_def;
DEFINE_HANDLE(actor_def);

typedef struct actor {
    vec2 pos;
    vec2 last_pos;
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

DECLARE_POOL(actor);
DECLARE_POOL(actor_def);

typedef struct actor_desc {
    vec2 pos;
    vec2 hsize;
    uint32_t sprite_id;
} actor_desc;

// public system interface

actor_handle actor_create(const actor_desc* const desc);
bool actor_destroy(actor_handle handle);

actor_def_handle actor_def_create(actor_def def);
bool actor_def_destroy(actor_def_handle handle);
actor_def* actor_def_get_name(char* name);
actor_def* actor_def_get_id(uint32_t name_id);

// game systems interface

void actor_system_init(game_settings* settings);
void actor_system_shutdown(void);
void actor_system_load_level(game_level* level);
void actor_system_unload_level(void);
void actor_system_update(float dt);
void actor_system_render(float rt);
void actor_system_config_ui(void);
void actor_system_debug_ui(void);
