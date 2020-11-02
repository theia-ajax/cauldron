#pragma once

#include "actor_system.h"
#include "game_systems_forward.h"
#include "handle.h"
#include "tx_math.h"
#include "tx_types.h"

// game systems interface

void bot_system_init(game_settings* settings);
void bot_system_shutdown(void);
void bot_system_load_level(game_level* level);
void bot_system_unload_level(void);
void bot_system_update(float dt);
void bot_system_render(float rt);

// public system structures
typedef enum bot_type {
    BotType_Default,
    BotType_Count,
} bot_type;

typedef struct bot {
    bot_type type;
    actor_handle actor;
    float dir;
    float jump_timer;
} bot;

DEFINE_HANDLE(bot);

typedef struct bot_desc {
    bot_type type;
} bot_desc;

HANDLE(bot) * bot_system_get_handles();
size_t bot_system_get_handles_len();
bot_handle bot_create(const bot_desc* const desc);
bool bot_destroy(bot_handle handle);
bot* bot_get(bot_handle handle);