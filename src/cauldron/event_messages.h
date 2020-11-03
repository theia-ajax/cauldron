#pragma once

#include "entity_desc.h"
#include "tx_math.h"
#include "tx_types.h"

typedef enum event_message_type {
    EventMessage_None,

    EventMessage_SpawnEntity,

    EventMessage_Count,
} event_message_type;

typedef struct event_message {
    event_message_type msg_type;
    void* user_data;
} event_message;

typedef struct spawn_entity_event {
    event_message event;
    entity_desc* entity_desc;
} spawn_entity_event;