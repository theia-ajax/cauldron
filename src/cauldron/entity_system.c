#include "entity_system.h"

#include "actor_system.h"
#include "bot_system.h"
#include "game_level.h"
#include "hash.h"

enum { MAX_ENTITIES = 128 };

struct {
    entity_handle ents[MAX_ENTITIES];
    actor_handle actors[MAX_ENTITIES];
    bot_handle bots[MAX_ENTITIES];
} ent_map;

uint16_t ent_gen = 1;

entity_created_cb* entity_created_subscribers = NULL;

void entity_system_init(game_settings* settings)
{
    memset(&ent_map, 0, sizeof(ent_map));
}

void entity_system_shutdown(void)
{
}

void entity_system_load_level(game_level* level)
{
    uint32_t ent_player_spawn = hash_string("PlayerSpawn");
    uint32_t ent_en01_spawn = hash_string("EnemySpawn01");

    for (int i = 0; i < arrlen(level->layer_insts); ++i) {
        game_layer_inst* layer = &level->layer_insts[i];
        if (layer->type == GAME_LAYER_TYPE_ENTITIES) {
            for (int j = 0; j < arrlen(layer->ents); ++j) {
                game_ent_def_inst* ent_def = &layer->ents[j];
                if (ent_def->id == ent_player_spawn) {
                    entity_create(&(entity_desc){
                        .actor_desc =
                            &(actor_desc){
                                .pos = {.x = ent_def->world_x, .y = ent_def->world_y},
                                .hsize = {.x = 0.45f, .y = 0.495f},
                                .sprite_id = 1,
                            },
                    });
                } else if (ent_def->id == ent_en01_spawn) {
                    entity_create(&(entity_desc){
                        .actor_desc =
                            &(actor_desc){
                                .pos = {.x = ent_def->world_x, .y = ent_def->world_y},
                                .hsize = {.x = 0.45f, .y = 0.375f},
                                .sprite_id = 2,
                            },
                        .bot_desc =
                            &(bot_desc){
                                .type = BotType_Default,
                            },
                    });
                }
            }
        }
    }
}

void entity_system_unload_level(void)
{
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        entity_destroy(ent_map.ents[i]);
        actor_destroy(ent_map.actors[i]);
        bot_destroy(ent_map.bots[i]);
    }
}

void entity_system_update(float dt)
{
}

void entity_system_render(float rt)
{
}

entity* entity_system_get_handles(void)
{
    return ent_map.ents;
}

size_t entity_system_get_handles_len(void)
{
    return MAX_ENTITIES;
}

entity entity_create(const entity_desc* const desc)
{
    if (desc) {
        int found = -1;
        for (int i = 0; i < MAX_ENTITIES; ++i) {
            if (!entity_handle_valid(ent_map.ents[i])) {
                found = i;
                break;
            }
        }

        if (found < 0) {
            return INVALID_HANDLE(entity);
        }

        entity ent = entity_handle_make((uint32_t)found, ent_gen);
        ent_map.ents[found] = ent;

        if (desc->actor_desc) {
            ent_map.actors[found] = actor_create(desc->actor_desc);
        }

        if (desc->bot_desc) {
            ent_map.bots[found] = bot_create(desc->bot_desc);
        }

        for (int i = 0; i < arrlen(entity_created_subscribers); ++i) {
            entity_created_subscribers[i](ent);
        }
    }
    return INVALID_HANDLE(entity);
}

bool entity_destroy(entity ent)
{
    if (entity_handle_valid(ent)) {
        uint32_t index = entity_handle_get_index(ent);

        actor_destroy(ent_map.actors[index]);
        bot_destroy(ent_map.bots[index]);

        return true;
    }

    return false;
}

void entity_system_subscribe_entity_created(entity_created_cb callback)
{
    arrput(entity_created_subscribers, callback);
}

actor_handle* entity_get_actor(entity ent)
{
    if (entity_handle_valid(ent)
        && actor_handle_valid(ent_map.actors[entity_handle_get_index(ent)])) {
        return &ent_map.actors[entity_handle_get_index(ent)];
    }
    return NULL;
}

bot_handle* entity_get_bot(entity ent)
{
    if (entity_handle_valid(ent) && bot_handle_valid(ent_map.bots[entity_handle_get_index(ent)])) {
        return &ent_map.bots[entity_handle_get_index(ent)];
    }
    return NULL;
}