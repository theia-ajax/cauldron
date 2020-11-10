#pragma once

#include "actor_system.h"
#include "game_systems_forward.h"
#include "tx_types.h"

tx_result player_system_init(game_settings* settings);
void player_system_term(void);
void player_system_update(float dt);

actor_handle get_player_actor(uint32_t pid);