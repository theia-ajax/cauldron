#pragma once

typedef struct game_level game_level;

void level_system_load_level(game_level* level);
void level_system_unload_level(void);
void level_system_render(float ft);