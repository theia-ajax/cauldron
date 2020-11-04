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

void entity_create(const entity_desc* const desc);

typedef void (*entity_created_cb)(entity);
