#pragma once

#include "entity_desc.h"
#include "game_systems_forward.h"
#include "handle.h"
#include "tx_types.h"

void entity_system_init(game_settings* settings);
void entity_system_shutdown(void);
void entity_system_load_level(game_level* level);
void entity_system_unload_level(void);
void entity_system_update(float dt);
void entity_system_render(float rt);

DEFINE_HANDLE(entity);
typedef entity_handle entity;

entity* entity_system_get_handles(void);
size_t entity_system_get_handles_len(void);

// #define INVALID_ENTITY ((entity){0})
// #define ENTITY_IS_VALID(entity) ((entity).value != 0)

entity entity_create(const entity_desc* const desc);
bool entity_destroy(entity ent);

typedef void (*entity_created_cb)(entity);

void entity_system_subscribe_entity_created(entity_created_cb callback);

typedef struct actor_handle actor_handle;
typedef struct bot_handle bot_handle;

actor_handle* entity_get_actor(entity ent);
bot_handle* entity_get_bot(entity ent);