#include "player_system.h"

#include "actor_system.h"
#include "entity_system.h"
#include "event_system.h"
#include "game_level.h"
#include "hash.h"
#include "sprite_draw.h"
#include "stb_ds.h"
#include "tx_input.h"
#include "tx_math.h"

struct player {
    actor_handle actor;
};

enum {
    MAX_PLAYERS = 4,
};

struct player players[MAX_PLAYERS];

static void on_entity_spawned(event_message* event)
{
    on_entity_spawned_event* on_entity_spawned = (on_entity_spawned_event*)event;

    uint32_t pid = on_entity_spawned->player_id;
    if (pid > 0) {
        int player_idx = (int)(pid - 1);
        if (VALID_INDEX(player_idx, MAX_PLAYERS)) {
            players[player_idx].actor = on_entity_spawned->h_actor;
        }
    }
}

void player_system_init(game_settings* settings)
{
    event_system_subscribe(EventMessage_OnEntitySpawned, on_entity_spawned);
}

void player_system_shutdown(void)
{
}

void player_system_load_level(game_level* level)
{
    // uint32_t key = hash_string("PlayerSpawn");

    // for (int i = 0; i < arrlen(level->layer_insts); ++i) {
    //     game_layer_inst* layer = &level->layer_insts[i];
    //     if (layer->type == GAME_LAYER_TYPE_ENTITIES) {
    //         for (int j = 0; j < arrlen(layer->ents); ++j) {
    //             game_ent_def_inst* ent_def = &layer->ents[j];
    //             if (ent_def->id == key) {
    //                 actor_handle handle = actor_create(&(actor_desc){
    //                     .pos = {.x = ent_def->world_x, .y = ent_def->world_y},
    //                     .hsize = {.x = 0.45f, .y = 0.495f},
    //                     .sprite_id = 1,
    //                 });
    //                 players[0].actor = handle;
    //             }
    //         }
    //     }
    // }
}

void player_system_unload_level(void)
{
}

void player_system_update(float dt)
{
    if (VALID_HANDLE(players[0].actor)) {
        vec2 input = {0};
        if (txinp_get_key(TXINP_KEY_LEFT)) input.x -= 1;
        if (txinp_get_key(TXINP_KEY_RIGHT)) input.x += 1;
        if (txinp_get_key(TXINP_KEY_UP)) input.y -= 1;
        if (txinp_get_key(TXINP_KEY_DOWN)) input.y += 1;

        actor* actor = actor_get(players[0].actor);
        if (actor) {
            actor->input.move = input;
            actor->input.jump = txinp_get_key_down(TXINP_KEY_Z);
        }
    }
}

void player_system_render(float rt)
{
}

actor_handle get_player_actor(uint32_t pid)
{
    if (VALID_INDEX(pid, 4)) {
        return players[pid].actor;
    } else {
        return INVALID_HANDLE(actor);
    }
}