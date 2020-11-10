#pragma once

#include "game_systems_forward.h"
#include "handle.h"
#include "hash.h"
#include "tx_types.h"

typedef tx_result (*game_system_init)(game_settings*);
typedef void (*game_system_term)(void);
typedef void (*game_system_load_level)(game_level* level);
typedef void (*game_system_unload_level)(void);
typedef void (*game_system_update)(float);
typedef void (*game_system_render)(float);

typedef struct game_system {
    char* name;
    game_system_init init;
    game_system_term term;
    game_system_load_level load_level;
    game_system_unload_level unload_level;
    game_system_update update;
    game_system_render render;
} game_system;

tx_result game_systems_init(game_settings* settings);
void game_systems_term(void);
void game_systems_load_level(game_level* level);
void game_systems_unload_level(void);
void game_systems_update(float dt);
void game_systems_render(float rt);

/* game systems must define an interface with the pattern

void X_system_init(game_settings* settings);
void X_system_term(void);
void X_system_load_level(game_level* level);
void X_system_unload_level(void);
void X_system_update(float dt);
void X_system_render(void);

*/