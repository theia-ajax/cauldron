#pragma once

#include "game_systems.h"
#include "tx_math.h"
#include "tx_types.h"

void phys_system_init(game_settings* settings);
void phys_system_shutdown(void);
void phys_system_load_level(game_level* level);
void phys_system_unload_level(void);
void phys_system_update(float dt);
void phys_system_render(void);

bool phys_solid(float x, float y, uint16_t layer_mask);
float phys_get_gravity();