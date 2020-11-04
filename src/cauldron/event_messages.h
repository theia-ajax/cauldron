#pragma once

#include "actor_system.h"
#include "bot_system.h"
#include "entity_desc.h"
#include "player_system.h"
#include "tx_math.h"
#include "tx_types.h"

typedef enum event_message_type {
    EventMessage_None,

    EventMessage_OnEntitySpawned,

    EventMessage_Count,
} event_message_type;

typedef struct event_message {
    event_message_type msg_type;
    void* user_data;
} event_message;

typedef struct on_entity_spawned_event {
    event_message event;
    uint32_t ent_id;
    actor_handle h_actor;
    bot_handle h_bot;
    uint32_t player_id;
} on_entity_spawned_event;