#pragma once

#include "game_systems_forward.h"
#include "handle.h"
#include "hash.h"
#include "tx_types.h"

typedef void (*game_system_init)(game_settings*);
typedef void (*game_system_shutdown)(void);
typedef void (*game_system_load_level)(game_level* level);
typedef void (*game_system_unload_level)(void);
typedef void (*game_system_update)(float);
typedef void (*game_system_render)(float);
typedef void (*game_system_config_ui)(void);
typedef void (*game_system_debug_ui)(void);

typedef struct game_system {
    char* name;
    game_system_init init;
    game_system_shutdown shutdown;
    game_system_load_level load_level;
    game_system_unload_level unload_level;
    game_system_update update;
    game_system_render render;
    game_system_config_ui config_ui;
    game_system_debug_ui debug_ui;
} game_system;

void game_systems_init(game_settings* settings);
void game_systems_shutdown(void);
void game_systems_load_level(game_level* level);
void game_systems_unload_level(void);
void game_systems_update(float dt);
void game_systems_render(float rt);
void game_systems_config_ui(void);
void game_systems_debug_ui(void);

/* game systems must define an interface with the pattern

void X_system_init(game_settings* settings);
void X_system_shutdown(void);
void X_system_load_level(game_level* level);
void X_system_unload_level(void);
void X_system_update(float dt);
void X_system_render(void);

*/