#include "actor_system.h"
#include "game_settings.h"
#include "phys_system.h"
#include "profile.h"
#include "sprite_draw.h"
#include "stb_ds.h"
#include "system_pool.h"
#include "tx_math.h"
#include <stdio.h>

// private system interface and structs
enum actor_move_result_flags {
    ActorMoveResultFlags_None = 0,
    ActorMoveResultFlags_StayGround = 1 << 0,
    ActorMoveResultFlags_StayWall = 1 << 1,
    ActorMoveResultFlags_StayCeiling = 1 << 2,
    ActorMoveResultFlags_HitGround = 1 << 3,
    ActorMoveResultFlags_HitWall = 1 << 4,
    ActorMoveResultFlags_HitCeiling = 1 << 5,
    ActorMoveResultFlags_LeftGround = 1 << 6,
    ActorMoveResultFlags_LeftWall = 1 << 7,
    ActorMoveResultFlags_LeftCeiling = 1 << 8,
};

struct actor_move_result {
    vec2 new_pos;
    vec2 new_vel;
    uint16_t flags;
};

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

typedef struct actor_def {
    vec2 hsize;
    uint32_t sprite_id;
    float jump_force;
    float max_speed;
    float move_accel;
    float grav_scale;
    float step_height;
} actor_def;

actor_def default_actor_def()
{
    return (actor_def){
        .hsize = {0.5f, 0.5f},
        .jump_force = 15.f,
        .max_speed = 10.0f,
        .move_accel = 5.0f,
        .grav_scale = 1.0f,
        .step_height = 0.125f,
    };
}

actor_def_handle h_default_actor_def;

// private system state

DEFINE_POOL(actor)
DEFINE_POOL(actor_def)

struct actor_def_entry {
    uint32_t key;
    actor_def_handle handle;
};
struct actor_def_entry* actor_defs_by_id = NULL;

actor_system_conf config;

actor_jump_report current_jump_report;
actor_jump_report* saved_jump_reports = NULL;

// private system interface

void start_jump_report(void);
void jump_record_frame(float pos_y, float vel_y, float acl_y, float dt);
void end_jump_report(void);
void save_last_jump_report(void);

struct actor_move_result actor_calc_move(const actor* actor, const actor_def* actdef, float dt);

// actor game systems interface implementation
tx_result actor_system_init(game_settings* settings)
{
    config = (actor_system_conf){
        .platform_drop_time = 0.1f,
        .jump_ungrounded_time = 0.05f,
    };

    actor_pool_set_capacity(64);
    actor_def_pool_set_capacity(16);

    actor_def defaults = default_actor_def();
    h_default_actor_def = actor_def_create("default", &defaults);

    actor_def_create(
        "PlayerSpawn",
        &(actor_def){
            .hsize = {.x = 0.45f, .y = 0.495f},
            .sprite_id = 1,
            .jump_force = 18.0f,
            .max_speed = 8.0f,
            .move_accel = 30.0f,
            .grav_scale = 1.0f,
            .step_height = 0.125f,
        });

    actor_def e01_spawn = default_actor_def();
    e01_spawn.hsize = (vec2){.x = 0.45f, .y = 0.375f};
    e01_spawn.sprite_id = 2;
    actor_def_create("EnemySpawn01", &e01_spawn);

    return TX_SUCCESS;
}

void actor_system_term(void)
{
    hmfree(actor_defs_by_id);

    actor_pool_free();
    actor_def_pool_free();
}

void actor_system_update(float dt)
{
    for (int i = 0; i < arrlen(actor_pool.data); ++i) {
        if (!VALID_HANDLE(actor_pool.handles[i])) {
            continue;
        }

        actor* actor = &actor_pool.data[i];
        actor_def* actdef = actor_def_ptr(actor->h_actor_def);

        // begin new movement
        actor->last_pos = actor->pos;

        // apply inputs
        const vec2 input_move = actor->input.move;

        if (input_move.x > 0) {
            actor->flags &= ~ActorFlags_FacingLeft;
        } else if (input_move.x < 0) {
            actor->flags |= ActorFlags_FacingLeft;
        }

        if (input_move.y > 0.0f) {
            actor->platform_timer = config.platform_drop_time;
        } else {
            if (actor->platform_timer > 0.0f) actor->platform_timer -= dt;
        }

        if (actor->jump_forgive_timer > 0.0f) {
            actor->jump_forgive_timer -= dt;
            if (actor->input.jump) {

                actor->vel.y = -actdef->jump_force;

                if (i == 0) start_jump_report();
            }
        }

        bool apply_friction = false;
        if (!near_zero(input_move.x)) {
            actor->vel.x += actdef->move_accel * dt * input_move.x;
        } else {
            apply_friction = true;
        }

        // apply physics
        actor->vel.x = clampf(actor->vel.x, -actdef->max_speed, actdef->max_speed);

        if (apply_friction) {
            if (actor->vel.x < 0) {
                actor->vel.x = min(actor->vel.x + 10.0f * dt, 0);
            } else if (actor->vel.x > 0) {
                actor->vel.x = max(actor->vel.x - 10.0f * dt, 0);
            }
        }

        if ((actor->flags & ActorFlags_OnGround) != 0 && actor->vel.y > 0.0f) {
            actor->vel.y = 8.0f;
            if (i == 0) end_jump_report();
        } else {
            actor->vel.y += phys_get_gravity() * dt * actdef->grav_scale;
        }

        if (i == 0) jump_record_frame(actor->pos.y, actor->vel.y, phys_get_gravity(), dt);

        // move the actor with physics
        struct actor_move_result move_result = actor_calc_move(actor, actdef, dt);

        actor->flags &= ~ActorFlags_AllMoveResultFlags;
        actor->flags |= move_result.flags;

        if ((actor->flags & ActorFlags_OnGround) != 0) {
            actor->jump_forgive_timer = config.jump_ungrounded_time;
        }

        // if ((move_result.flags & ActorMoveResultFlags_HitWall) != 0) printf("wall\n");
        // if ((move_result.flags & ActorMoveResultFlags_HitGround) != 0) printf("floor\n");
        // if ((move_result.flags & ActorMoveResultFlags_HitCeiling) != 0) printf("ceiling\n");

        actor->pos = move_result.new_pos;
        actor->vel = move_result.new_vel;
    }
}

struct actor_move_result actor_calc_move(const actor* actor, const actor_def* actdef, float dt)
{
    const bool was_contact_ground = (actor->flags & ActorFlags_OnGround) != 0;
    const bool was_contact_wall = (actor->flags & ActorFlags_OnWall) != 0;
    const bool was_contact_ceiling = (actor->flags & ActorFlags_OnCeiling) != 0;

    const float max_step_dist = 0.1f;
    const float solid_scan_dist = 0.01f;

    vec2 total_delta = vec2_scale(actor->vel, dt);
    vec2 new_pos = actor->pos;
    vec2 new_vel = actor->vel;
    uint16_t move_flags = ActorMoveResultFlags_None;
    vec2 remaining_delta = vec2_abs(total_delta);
    int safety_valve = 1000;

    while ((remaining_delta.x > FLT_EPSILON || remaining_delta.y > FLT_EPSILON)
           && safety_valve-- > 0) {
        vec2 dir = vec2_norm(total_delta);
        float remaining_dist = vec2_len(remaining_delta);
        float dist_to_travel = fminf(max_step_dist, remaining_dist);
        vec2 delta = vec2_scale(dir, dist_to_travel);

        // horizontal movement
        {
            float check_x = new_pos.x + delta.x + signf(delta.x) * actdef->hsize.x;
            float bottom = new_pos.y - 0.25f;
            float top = new_pos.y - actdef->hsize.y * 1.75f;
            float center = new_pos.y - actdef->hsize.y;
            if (!phys_solid(check_x, center, 1) /* && !phys_solid(check_x, top, 1)*/) {
                new_pos.x += delta.x;
                remaining_delta.x -= fabsf(delta.x);
            } else {
                while (!phys_solid(new_pos.x + signf(delta.x) * actdef->hsize.x, center, 1)
                       /* && !phys_solid(new_pos.x + signf(delta.x) * actor->hsize.x, top, 1) */) {
                    new_pos.x += signf(delta.x) * solid_scan_dist;
                }
                new_vel.x = 0.0f;
                remaining_delta.x = 0.0f;
                move_flags |= ActorMoveResultFlags_StayWall;
                if (!was_contact_wall) {
                    move_flags |= ActorMoveResultFlags_HitWall;
                }
            }
        }

        // vertical movement
        {
            float left = new_pos.x - actdef->hsize.x * 0.95f;
            float right = new_pos.x + actdef->hsize.x * 0.95f;

            if (delta.y >= 0.0f) {
                // going down
                uint16_t ground_mask =
                    (actor->vel.y < 0.0f || actor->platform_timer > 0.0f) ? 1 : 3;
                float bottom = new_pos.y;
                float top = new_pos.y - actdef->hsize.y * 2.0f;

                if (phys_solid(left, bottom + delta.y, ground_mask)
                    || phys_solid(right, bottom + delta.y, ground_mask)) {
                    // snap down
                    while (!phys_solid(left, new_pos.y, ground_mask)
                           && !phys_solid(right, new_pos.y, ground_mask)) {
                        new_pos.y += solid_scan_dist;
                    }

                    // pop up
                    while (phys_solid(left, new_pos.y - 0.125f, ground_mask)
                           || phys_solid(right, new_pos.y - 0.125f, ground_mask)
                           || phys_solid(new_pos.x, new_pos.y, ground_mask)) {
                        new_pos.y -= solid_scan_dist;
                    }

                    remaining_delta.y = 0.0f;
                    move_flags |= ActorMoveResultFlags_StayGround;
                    if (!was_contact_ground) {
                        move_flags |= ActorMoveResultFlags_HitGround;
                    }
                } else {
                    new_pos.y += delta.y;
                    remaining_delta.y -= fabsf(delta.y);
                    if (was_contact_ground && new_vel.y > 0.0f) {
                        new_vel.y = 0.0f;
                    }
                }
            } else {
                // going up
                if (phys_solid(left, new_pos.y - delta.y - actdef->hsize.y * 2.0f, 1)
                    || phys_solid(right, new_pos.y - delta.y - actdef->hsize.y * 2.0f, 1)) {

                    // find contact point
                    while (!phys_solid(left, new_pos.y - actdef->hsize.y * 2.0f, 1)
                           && !phys_solid(right, new_pos.y - actdef->hsize.y * 2.0f, 1)) {
                        new_pos.y -= solid_scan_dist;
                    }

                    new_vel.y = 0.0f;
                    remaining_delta.y = 0.0f;
                    move_flags |= ActorMoveResultFlags_StayCeiling;
                    if (!was_contact_ceiling) {
                        move_flags |= ActorMoveResultFlags_HitCeiling;
                    }
                } else {
                    new_pos.y += delta.y;
                    remaining_delta.y -= fabsf(delta.y);
                }
            }
        }

        if (was_contact_wall && (move_flags & ActorMoveResultFlags_StayWall) == 0) {
            move_flags |= ActorMoveResultFlags_LeftWall;
        }
        if (was_contact_ground && (move_flags & ActorMoveResultFlags_StayGround) == 0) {
            move_flags |= ActorMoveResultFlags_LeftGround;
        }
        if (was_contact_ceiling && (move_flags & ActorMoveResultFlags_StayCeiling) == 0) {
            move_flags |= ActorMoveResultFlags_LeftCeiling;
        }
    }

    if (safety_valve <= 0) {
        printf("actor move safety valve limit exceeded.\n");
    }

    return (struct actor_move_result){
        .new_pos = new_pos,
        .new_vel = new_vel,
        .flags = move_flags,
    };
}

void start_jump_report(void)
{
    if (!current_jump_report.track_enabled) {
        end_jump_report();
    }

    current_jump_report.track_enabled = true;
    current_jump_report.current_frame = 0;

    if (current_jump_report.jump_frames) {
        arrfree(current_jump_report.jump_frames);
        current_jump_report.jump_frames = NULL;
    }
}

void jump_record_frame(float pos_y, float vel_y, float acl_y, float dt)
{
    if (!current_jump_report.track_enabled) {
        return;
    }

    arrput(
        current_jump_report.jump_frames,
        ((actor_jump_frame_report){
            .frame = current_jump_report.current_frame,
            .ticks = get_ticks(),
            .pos_y = pos_y,
            .vel_y = vel_y,
            .acl_y = acl_y,
            .dt = dt,
        }));

    current_jump_report.current_frame++;
}

void end_jump_report(void)
{
    current_jump_report.track_enabled = false;
}

void save_last_jump_report(void)
{
    if (!current_jump_report.track_enabled) {
        actor_jump_frame_report* copy = NULL;
        size_t len = arrlen(current_jump_report.jump_frames);
        arrsetlen(copy, len);
        for (int i = 0; i < len; ++i) {
            copy[i] = current_jump_report.jump_frames[i];
        }
        actor_jump_report jr = current_jump_report;
        jr.jump_frames = copy;
        arrput(saved_jump_reports, jr);
    }
}

void actor_system_render(float rt)
{
    for (int i = 0; i < arrlen(actor_pool.data); ++i) {
        if (!VALID_HANDLE(actor_pool.handles[i])) {
            continue;
        }

        actor* actor = &actor_pool.data[i];

        sprite_flip flip =
            ((actor->flags & ActorFlags_FacingLeft) != 0) ? SPRITE_FLIP_X : SPRITE_FLIP_NONE;

        vec2 delta = vec2_sub(actor->pos, actor->last_pos);

        actor_def* actdef = actor_def_ptr(actor->h_actor_def);

        spr_draw(&(sprite_draw_desc){
            .sprite_id = actdef->sprite_id,
            .pos = vec2_add(actor->pos, vec2_scale(delta, rt)),
            .origin = {.x = 0.5f, .y = 1.0f},
            .layer = -5.0f,
            .flip = flip,
        });
    }
}

// public system implementation

// actor implementation
actor_handle actor_create(const actor_desc* const desc)
{
    if (desc) {
        actor_handle handle = actor_acquire();
        if (VALID_HANDLE(handle)) {
            uint32_t index = actor_handle_get_index(handle);
            actor_def_handle h_actor_def =
                (VALID_HANDLE(desc->h_actor_def)) ? desc->h_actor_def : h_default_actor_def;
            actor_pool.data[index] = (actor){
                .pos = desc->pos,
                .h_actor_def = h_actor_def,
            };
            return handle;
        }
    }
    return INVALID_HANDLE(actor);
}

bool actor_destroy(actor_handle handle)
{
    if (actor_handle_valid(handle)) {
        actor_release(handle);
        return true;
    }
    return false;
}

// actor_def implementation
actor_def_handle actor_def_create(char* name, actor_def* def)
{
    if (def) {
        actor_def_handle handle = actor_def_acquire();
        if (VALID_HANDLE(handle)) {
            uint32_t index = actor_def_handle_get_index(handle);
            hmputs(
                actor_defs_by_id,
                ((struct actor_def_entry){
                    .key = hash_string(name),
                    .handle = handle,
                }));
            actor_def_pool.data[index] = *def;
            return handle;
        }
    }
    return INVALID_HANDLE(actor_def);
}

bool actor_def_destroy(actor_def_handle handle)
{
    if (actor_def_handle_valid(handle)) {
        actor_def_release(handle);
        return true;
    }
    return false;
}

actor_def_handle actor_def_get_name(char* name)
{
    if (name) {
        return actor_def_get_id(hash_string(name));
    } else {
        return INVALID_HANDLE(actor_def);
    }
}

actor_def_handle actor_def_get_id(uint32_t name_id)
{
    return hmgets(actor_defs_by_id, name_id).handle;
}

// imgui editors

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

void jump_report_ui(actor_jump_report* report)
{
    for (int i = 0; i < arrlen(report->jump_frames); ++i) {
        actor_jump_frame_report frame = report->jump_frames[i];
        float pd = 0.0f;
        if (i > 0) {
            pd = frame.pos_y - report->jump_frames[i - 1].pos_y;
        }
        igText("p:%0.5f v:%0.5f pd:%0.5f m:%f", frame.pos_y, frame.vel_y, pd, pd / frame.dt);

        // igText(
        //     "%d : time:%llu p:%0.2f v:%0.2f a:%0.2f dt:%f",
        //     frame.frame,
        //     frame.ticks,
        //     frame.pos_y,
        //     frame.vel_y,
        //     frame.acl_y,
        //     frame.dt);
    }

    igSeparator();
}

void jump_report_window(void)
{
    igBegin("Jumps", NULL, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    igBeginChildStr(
        "Current Report",
        (ImVec2){igGetWindowContentRegionWidth() * 0.25f, 0},
        true,
        ImGuiWindowFlags_None);
    {
        if (igButton("Save Last", (ImVec2){0})) {
            save_last_jump_report();
        }

        jump_report_ui(&current_jump_report);
    }
    igEndChild();

    igSameLine(0.0f, -1.0f);

    igBeginChildStr("Saved Reports", (ImVec2){0}, true, ImGuiWindowFlags_None);
    {
        size_t len = arrlen(saved_jump_reports);
        static int cur_idx = 0;

        if (len > 0) {
            igInputInt("Saved Id", &cur_idx, 1, 10, ImGuiInputTextFlags_None);
            cur_idx = mod(cur_idx, (int)len);
            jump_report_ui(&saved_jump_reports[cur_idx]);
        }
    }
    igEndChild();

    igEnd();
}

void actor_def_config_ui(actor_def_handle sel_handle)
{
    actor_def* actdef = actor_def_ptr(sel_handle);
    // float jump_force;
    //     float max_speed;
    //     float move_accel;
    //     float grav_scale;
    //     float step_height;
    igInputFloat2("half size", &actdef->hsize.x, "%0.3f", ImGuiInputTextFlags_CharsDecimal);
    igInputFloat(
        "gravity scale", &actdef->grav_scale, 0.1f, 0.5f, "%0.2f", ImGuiInputTextFlags_None);
    igInputFloat("jump force", &actdef->jump_force, 0.1f, 0.5f, "%0.2f", ImGuiInputTextFlags_None);
    igInputFloat("max speed", &actdef->max_speed, 0.1f, 0.5f, "%0.2f", ImGuiInputTextFlags_None);
    igInputFloat(
        "acceleration", &actdef->move_accel, 0.1f, 0.5f, "%0.2f", ImGuiInputTextFlags_None);
}

void actor_def_editor_window(bool* show_editor)
{
    if (igBegin("Actor Definitions", show_editor, ImGuiWindowFlags_None)) {
        static actor_def_handle sel_handle = {INVALID_RAW_HANDLE};
        if (igBeginTabBar("def bar", ImGuiTabBarFlags_None)) {
            actor_def_handle* handles = get_actor_def_handles();
            size_t len = get_actor_def_handles_len();

            for (size_t i = 0; i < len; ++i) {
                actor_def_handle handle = handles[i];
                if (actor_def_handle_valid(handle)) {
                    actor_def* actdef = actor_def_ptr(handle);
                    char buffer[4] = {0};
                    snprintf(buffer, 4, "%llu", i);
                    ImGuiTabItemFlags tab_flags = ImGuiTabItemFlags_Button;
                    if (handle.value == sel_handle.value) {
                        tab_flags |= ImGuiTabItemFlags_SetSelected;
                    }
                    if (igTabItemButton(buffer, tab_flags)) {
                        sel_handle = handle;
                    }
                }
            }

            igEndTabBar();
        }
        if (VALID_HANDLE(sel_handle)) {
            actor_def_config_ui(sel_handle);
        }
        igEnd();
    }
}