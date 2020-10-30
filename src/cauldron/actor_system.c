#include "actor_system.h"
#include "game_settings.h"
#include "phys_system.h"
#include "profile.h"
#include "sprite_draw.h"
#include "stb_ds.h"
#include "tx_math.h"
#include <stdio.h>

// private system state
actor* actors = NULL;
actor_handle* handles = NULL;
uint32_t* free_stack = NULL;
uint16_t actors_gen = 1;

actor_system_conf config;

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

struct actor_move_result actor_calc_move(actor* actor, float dt)
{
    const bool was_contact_ground = (actor->flags & ActorFlags_OnGround) != 0;
    const bool was_contact_wall = (actor->flags & ActorFlags_OnWall) != 0;
    const bool was_contact_ceiling = (actor->flags & ActorFlags_OnCeiling) != 0;

    const float max_step_dist = 0.1f;
    const float solid_scan_dist = 0.01f;

    vec2 total_delta = vec2_scale(actor->vel, dt);
    vec2 new_pos = actor->pos;
    vec2 new_vel = actor->vel;
    uint8_t move_flags = ActorMoveResultFlags_None;
    vec2 remaining_delta = vec2_abs(total_delta);
    int safety_valve = 100;

    while ((remaining_delta.x > FLT_EPSILON || remaining_delta.y > FLT_EPSILON)
           && safety_valve-- > 0) {
        vec2 dir = vec2_norm(total_delta);
        float remaining_dist = vec2_len(remaining_delta);
        float dist_to_travel = fminf(max_step_dist, remaining_dist);
        vec2 delta = vec2_scale(dir, dist_to_travel);

        // horizontal movement
        {
            float check_x = new_pos.x + delta.x + signf(delta.x) * actor->hsize.x;
            float bottom = new_pos.y - 0.25f;
            float top = new_pos.y - actor->hsize.y * 1.75f;
            float center = new_pos.y - actor->hsize.y;
            if (!phys_solid(check_x, center, 1) /* && !phys_solid(check_x, top, 1)*/) {
                new_pos.x += delta.x;
                remaining_delta.x -= fabsf(delta.x);
            } else {
                while (!phys_solid(new_pos.x + signf(delta.x) * actor->hsize.x, center, 1)
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
            float left = new_pos.x - actor->hsize.x * 0.95f;
            float right = new_pos.x + actor->hsize.x * 0.95f;

            if (delta.y >= 0.0f) {
                // going down
                uint16_t ground_mask =
                    (actor->vel.y < 0.0f || actor->platform_timer > 0.0f) ? 1 : 3;
                float bottom = new_pos.y;
                float top = new_pos.y - actor->hsize.y * 2.0f;

                if (phys_solid(left, bottom + delta.y, ground_mask)
                    || phys_solid(right, bottom + delta.y, ground_mask)) {
                    actor->flags |= ActorFlags_OnGround;

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
                if (phys_solid(left, new_pos.y - delta.y - actor->hsize.y * 2.0f, 1)
                    || phys_solid(right, new_pos.y - delta.y - actor->hsize.y * 2.0f, 1)) {

                    // find contact point
                    while (!phys_solid(left, new_pos.y - actor->hsize.y * 2.0f, 1)
                           && !phys_solid(right, new_pos.y - actor->hsize.y * 2.0f, 1)) {
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

bool pop_free_index(uint32_t* out)
{
    size_t len = arrlen(free_stack);
    if (len > 0) {
        if (out) {
            *out = free_stack[len - 1];
        }
        arrdel(free_stack, len - 1);
        return true;
    }
    return false;
}

void push_free_index(uint32_t index)
{
    arrput(free_stack, index);
}

actor_handle actor_make_handle(uint32_t index, uint32_t gen)
{
    return (actor_handle){(gen << 16) + (index & 0xFFFF)};
}

uint32_t actor_handle_index(actor_handle handle)
{
    return handle.value & 0xFFFF;
}

inline uint16_t actor_handle_gen(actor_handle handle)
{
    return (uint16_t)(handle.value >> 16);
}

inline bool actor_handle_valid(actor_handle handle)
{
    return handle.value != 0;
}

// actor game systems interface implementation
void actor_system_init(game_settings* settings)
{
    config = (actor_system_conf){
        .platform_drop_time = 0.1f,
        .jump_ungrounded_time = 0.05f,
    };

    arrsetlen(actors, 64);
    arrsetlen(handles, 64);
    arrsetcap(free_stack, 64);

    memset(actors, 0, sizeof(actor) * arrlen(actors));
    memset(handles, 0, sizeof(actor_handle) * arrlen(handles));

    for (int i = 63; i >= 0; --i) {
        arrput(free_stack, i);
    }
}

void actor_system_shutdown(void)
{
    arrfree(actors);
    arrfree(handles);
    arrfree(free_stack);
}

void actor_system_load_level(game_level* level)
{
}

void actor_system_unload_level(void)
{
}

void actor_system_update(float dt)
{
    for (int i = 0; i < arrlen(actors); ++i) {
        if (!VALID_HANDLE(handles[i])) {
            continue;
        }

        actor* actor = &actors[i];

        // update timers
        if (actor->platform_timer > 0.0f) actor->platform_timer -= dt;

        const vec2 input_move = actor->input.move;

        if (input_move.x > 0) {
            actor->flags &= ~ActorFlags_FacingLeft;
        } else if (input_move.x < 0) {
            actor->flags |= ActorFlags_FacingLeft;
        } else {
            if (actor->vel.x < 0) {
                actor->vel.x = min(actor->vel.x + 10.0f * dt, 0);
            } else if (actor->vel.x > 0) {
                actor->vel.x = max(actor->vel.x - 10.0f * dt, 0);
            }
        }

        if (input_move.y > 0.0f) {
            actor->platform_timer = config.platform_drop_time;
        }

        actor->vel.x += 30.0f * dt * input_move.x;
        actor->vel.x = clampf(actor->vel.x, -8.0f, 8.0f);

        if ((actor->flags & ActorFlags_OnGround) != 0 && actor->vel.y > 0.0f) {
            actor->vel.y = 8.0f;
        } else {
            const float gravity = 1.0f; // phys_get_gravity();
            actor->vel.y += gravity * dt;
        }

        if (actor->jump_forgive_timer > 0.0f) {
            actor->jump_forgive_timer -= dt;
            if (actor->input.jump) {
                actor->vel.y = -1.0f;
                actor->track_jump.enable = true;
                actor->track_jump.start_time = get_ticks();
                actor->track_jump.start_y = actor->pos.y;
                actor->track_jump.min_y = FLT_MAX;
            }
        }

        struct actor_move_result move_result = actor_calc_move(actor, dt);

        actor->flags &= ~0xE;
        actor->flags |= ((move_result.flags & 0x7) << 1);

        if ((actor->flags & ActorFlags_OnGround) != 0) {
            actor->jump_forgive_timer = config.jump_ungrounded_time;
        }

        // if ((move_result.flags & ActorMoveResultFlags_HitWall) != 0) printf("wall\n");
        // if ((move_result.flags & ActorMoveResultFlags_HitGround) != 0) printf("floor\n");
        // if ((move_result.flags & ActorMoveResultFlags_HitCeiling) != 0) printf("ceiling\n");

        actor->pos = move_result.new_pos;
        actor->vel = move_result.new_vel;

        if (actor->track_jump.enable) {
            if (actor->pos.y < actor->track_jump.min_y) {
                actor->track_jump.min_y = actor->pos.y;
            }
            if ((move_result.flags & ActorMoveResultFlags_HitGround) != 0) {
                actor->track_jump.enable = false;

                float jump_height = actor->pos.y - actor->track_jump.min_y;
                uint64_t jump_time =
                    (get_ticks() - actor->track_jump.start_time) * 1000 / get_frequency();

                printf(
                    "Jump\n\tStart Y:\t%0.3f\n\tHeight:\t\t%0.3f\n\tTime:\t\t%llums\n\n",
                    actor->track_jump.start_y,
                    jump_height,
                    jump_time);
            }
        }
    }
}

void actor_system_render(void)
{
    for (int i = 0; i < arrlen(actors); ++i) {
        actor* actor = &actors[i];

        sprite_flip flip =
            ((actor->flags & ActorFlags_FacingLeft) != 0) ? SPRITE_FLIP_X : SPRITE_FLIP_NONE;

        spr_draw(&(sprite_draw_desc){
            .sprite_id = actor->sprite_id,
            .pos = actor->pos,
            .origin = {.x = 0.5f, .y = 1.0f},
            .layer = -5.0f,
            .flip = flip,
        });
    }
}

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

void actor_system_config_ui(void)
{
    igInputFloat(
        "Platform Drop Time",
        &config.platform_drop_time,
        0.01f,
        0.1f,
        "%0.2f seconds",
        ImGuiInputTextFlags_None);

    igInputFloat(
        "Jump Ungrounded Time",
        &config.jump_ungrounded_time,
        0.01f,
        0.1f,
        "%0.2f seconds",
        ImGuiInputTextFlags_None);
}

void actor_system_debug_ui(void)
{
}

actor_handle actor_create(actor_desc* desc)
{
    if (desc) {
        uint32_t index;
        if (pop_free_index(&index)) {
            handles[index] = actor_make_handle(index, actors_gen);
            actors[index] = (actor){
                .pos = desc->pos,
                .hsize = desc->hsize,
                .sprite_id = desc->sprite_id,
            };
            return handles[index];
        }
    }
    return INVALID_HANDLE(actor);
}

void actor_destroy(actor_handle handle)
{
    actor* actor = actor_get(handle);
    if (actor) {
        ptrdiff_t index = actor - actors;
        uint16_t gen = actor_handle_gen(handles[index]);
        handles[index] = INVALID_HANDLE(actor);
        push_free_index((uint32_t)index);
        if (gen >= actors_gen) {
            ++actors_gen;
        }
    }
}

actor* actor_get(actor_handle handle)
{
    if (actor_handle_valid(handle)) {
        uint32_t index = actor_handle_index(handle);

        if (index < arrlen(actors)) {
            if (actor_handle_gen(handle) == actor_handle_gen(handles[index])) {
                return &actors[index];
            }
        }
    }
    return NULL;
}
