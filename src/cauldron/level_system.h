#pragma once

#include "game_systems_forward.h"
#include "strhash.h"
#include "tx_types.h"

tx_result level_system_init(game_settings* settings);
void level_system_term(void);
void level_system_load_level(game_level* level);
void level_system_unload_level(void);
void level_system_render(float ft);
void level_load_name(char* name);
void level_load_id(strhash id);
void level_select_window(bool* show);