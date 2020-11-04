#pragma once

#include "tx_types.h"

typedef struct game_ent_def_inst game_ent_def_inst;
typedef struct actor_desc actor_desc;
typedef struct bot_desc bot_desc;

typedef struct entity_desc {
    game_ent_def_inst* ent_def;
    actor_desc* actor_desc;
    bot_desc* bot_desc;
    uint32_t player_id;
} entity_desc;