#pragma once

#include "game_systems_forward.h"
#include "tx_types.h"

void player_system_init(game_settings* settings);
void player_system_shutdown(void);
void player_system_load_level(game_level* level);
void player_system_unload_level(void);
void player_system_update(float dt);
void player_system_render(void);