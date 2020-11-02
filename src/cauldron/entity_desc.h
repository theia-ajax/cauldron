#pragma once

#include "tx_types.h"

typedef struct actor_desc actor_desc;
typedef struct bot_desc bot_desc;

typedef struct entity_desc {
    actor_desc* actor_desc;
    bot_desc* bot_desc;
} entity_desc;