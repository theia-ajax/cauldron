#include "actor_system.h"
#include "game_settings.h"
#include "phys_system.h"
#include "sprite_draw.h"
#include "stb_ds.h"
#include "tx_math.h"

actor* actors = NULL;
actor_handle* handles = NULL;
uint32_t* free_stack = NULL;
uint16_t actors_gen = 1;

void actor_system_init(game_settings* settings)
{
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
        actor* actor = &actors[i];

        const vec2 input_move = actor->input.move;

        float left = actor->pos.x - actor->hsize.x;
        float right = actor->pos.x + actor->hsize.x;
        float bottom = actor->pos.y;
        float top = actor->pos.y - actor->hsize.y * 2.0f;

        if (input_move.x > 0) {
            actor->flags &= ~ACTOR_FLAGS_FACING_LEFT;
        } else if (input_move.x < 0) {
            actor->flags |= ACTOR_FLAGS_FACING_LEFT;
        } else {
            if (actor->vel.x < 0) {
                actor->vel.x = min(actor->vel.x + 10.0f * dt, 0);
            } else if (actor->vel.x > 0) {
                actor->vel.x = max(actor->vel.x - 10.0f * dt, 0);
            }
        }

        uint16_t mask = (actor->vel.y < 0.0f || actor->input.move.y > 0.0f) ? 1 : 3;

        const float scan = 0.01f;
        if (phys_solid(left, bottom + scan, mask) || phys_solid(right, bottom + scan, mask)) {
            actor->flags |= ACTOR_FLAGS_GROUNDED;
        } else {
            actor->flags &= ~ACTOR_FLAGS_GROUNDED;
        }

        if (actor->input.jump && (actor->flags & ACTOR_FLAGS_GROUNDED) != 0) {
            actor->vel.y = -21.0f;
        }

        mask = (actor->vel.y < 0.0f || actor->input.move.y > 0.0f) ? 1 : 3;

        actor->vel.x += 30.0f * dt * input_move.x;
        actor->vel.x = clampf(actor->vel.x, -8.0f, 8.0f);
        actor->vel.y += phys_get_gravity() * dt;

        if ((actor->flags & ACTOR_FLAGS_GROUNDED) != 0 && actor->vel.y > 0.0f) {
            actor->vel.y = 0.0f;
        }

        vec2 delta = vec2_scale(actor->vel, dt);

        while (delta.x > 0.0f
               && (phys_solid(right + delta.x, top, mask)
                   || phys_solid(right + delta.x, bottom, mask))) {
            delta.x = max(delta.x - 0.125f / 2.0f, 0.0f);
        }

        while (delta.x < 0.0f
               && (phys_solid(left + delta.x, top, mask)
                   || phys_solid(left + delta.x, bottom, mask))) {
            delta.x = min(delta.x + 0.125f / 2.0f, 0.0f);
        }

        while (delta.y > 0.0f
               && (phys_solid(left, bottom + delta.y, mask)
                   || phys_solid(right, bottom + delta.y, mask))) {
            delta.y = max(delta.y - 0.125f / 2.0f, 0.0f);
        }

        while (
            delta.y < 0.0f
            && (phys_solid(left, top + delta.y, mask) || phys_solid(right, top + delta.y, mask))) {
            delta.y = min(delta.y + 0.125f / 2.0f, 0.0f);
        }

        actor->pos = vec2_add(delta, actor->pos);
        actor->vel = vec2_scale(delta, 1.0f / dt);
    }

    /*

    float dx = actor->velocity.x * dt, dy = actor->velocity.y * dt;

    const float scan_dist = 0.1f;

    while (dx > 0.0f
           && (map::is_point_solid(map, right + dx, top)
               || map::is_point_solid(map, right + dx, bottom))) {
        dx = std::max(dx - scan_dist, 0.0f);
    }

    while (dx < 0.0f
           && (map::is_point_solid(map, left + dx, top)
               || map::is_point_solid(map, left + dx, bottom))) {
        dx = std::min(dx + scan_dist, 0.0f);
    }

    while (dy > 0.0f
           && (map::is_point_solid(map, right, bottom + dy)
               || map::is_point_solid(map, left, bottom + dy))) {
        dy = std::max(dy - scan_dist, 0.0f);
    }

    while (dy < 0.0f
           && (map::is_point_solid(map, right, top + dy)
               || map::is_point_solid(map, left, top + dy))) {
        dy = std::min(dy + scan_dist, 0.0f);
    }

    actor->velocity = glm::vec2(dx, dy);
    actor->position += actor->velocity;
    actor->velocity /= dt;
    */
}

void actor_system_render(void)
{
    for (int i = 0; i < arrlen(actors); ++i) {
        actor* actor = &actors[i];

        sprite_flip flip =
            ((actor->flags & ACTOR_FLAGS_FACING_LEFT) != 0) ? SPRITE_FLIP_X : SPRITE_FLIP_NONE;

        spr_draw(&(sprite_draw_desc){
            .sprite_id = actor->sprite_id,
            .pos = actor->pos,
            .origin = {.x = 0.5f, .y = 1.0f},
            .layer = -5.0f,
            .flip = flip,
        });
    }
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

actor_handle actor_make_handle(uint32_t index)
{
    return (actor_handle){(actors_gen << 16) + (index & 0xFFFF)};
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

actor_handle actor_create(actor_desc* desc)
{
    if (desc) {
        uint32_t index;
        if (pop_free_index(&index)) {
            handles[index] = actor_make_handle(index);
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
