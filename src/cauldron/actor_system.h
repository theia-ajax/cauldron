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
void actor_system_render(float rt);
void actor_system_config_ui(void);
void actor_system_debug_ui(void);

typedef struct actor_system_conf {
    // how much time after the actor has input a platform-drop input do we keep platform-drop mode
    // on
    float platform_drop_time;

    // how much time after the actor becomes ungrounded is the actor still allowed to jump as if
    // they are grounded.
    float jump_ungrounded_time;

    float jump_force;
    float max_speed;
    float move_accel;
    float grav_scale;
    float step_height;
} actor_system_conf;

// public system structures

typedef struct actor_jump_frame_report {
    uint32_t frame; // current frame
    uint64_t ticks;
    float pos_y; // frame y position
    float vel_y; // frame y velocity
    float acl_y; // frame y acceleration (gravity)
    float dt;    // frame delta time
} actor_jump_frame_report;

typedef struct actor_jump_report {
    bool track_enabled;
    uint32_t current_frame;
    actor_jump_frame_report* jump_frames; // stbds_arr
} actor_jump_report;

enum actor_flags {
    ActorFlags_FacingLeft = 1 << 0,
    ActorFlags_OnGround = 1 << 1,
    ActorFlags_OnWall = 1 << 2,
    ActorFlags_OnCeiling = 1 << 3,
};

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
    struct {
        bool enable;
        float start_y;
        uint64_t start_time;
        float min_y;
    } track_jump;
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
