#include "entity_system.h"

#include "actor_system.h"
#include "bot_system.h"
#include "event_messages.h"
#include "event_system.h"
#include "game_level.h"
#include "hash.h"
#include <stb_ds.h>

typedef void (*spawn_entity_proc)(entity_desc*);

typedef struct entity_spawn_data {
    uint32_t key;
    spawn_entity_proc spawn_proc;
} entity_spawn_data;

entity_spawn_data* ent_spawn_data = NULL;

void spawn_entity_player_spawn(entity_desc* desc)
{
    game_ent_def_inst* ent_def = desc->ent_def;
    desc->actor_desc = &(actor_desc){
        .pos = {.x = ent_def->world_x, .y = ent_def->world_y},
        .hsize = {.x = 0.45f, .y = 0.495f},
        .sprite_id = 1,
    };
    desc->player_id = 1;

    entity_create(desc);
}

void spawn_entity_enemy_01_spawn(entity_desc* desc)
{
    game_ent_def_inst* ent_def = desc->ent_def;
    desc->actor_desc = &(actor_desc){
        .pos = {.x = ent_def->world_x, .y = ent_def->world_y},
        .hsize = {.x = 0.45f, .y = 0.375f},
        .sprite_id = 2,
    };
    desc->bot_desc = &(bot_desc){
        .type = BotType_Default,
    };

    entity_create(desc);
}

void register_entity(char* name, spawn_entity_proc spawn_proc)
{
    uint32_t key = hash_string(name);
    stbds_hmputs(
        ent_spawn_data,
        ((entity_spawn_data){
            .key = hash_string(name),
            .spawn_proc = spawn_proc,
        }));
}

void spawn_entity(entity_desc* desc)
{
    if (desc && desc->ent_def) {
        entity_spawn_data spawn_data = hmgets(ent_spawn_data, desc->ent_def->id);
        if (spawn_data.spawn_proc) {
            spawn_data.spawn_proc(desc);
        }
    }
}

void entity_system_init(game_settings* settings)
{
    register_entity("PlayerSpawn", spawn_entity_player_spawn);
    register_entity("EnemySpawn01", spawn_entity_enemy_01_spawn);
}

void entity_system_shutdown(void)
{
    hmfree(ent_spawn_data);
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
                spawn_entity(&(entity_desc){.ent_def = ent_def});
            }
        }
    }
}

void entity_system_unload_level(void)
{
}

void entity_system_update(float dt)
{
}

void entity_system_render(float rt)
{
}

void entity_create(const entity_desc* const desc)
{
    if (desc) {
        on_entity_spawned_event event = (on_entity_spawned_event){
            .event.msg_type = EventMessage_OnEntitySpawned,
            .ent_id = desc->ent_def->id,
            .h_actor = actor_create(desc->actor_desc),
            .h_bot = bot_create(desc->bot_desc),
            .player_id = desc->player_id,
        };

        event_send((event_message*)&event);
    }
}
