#pragma once

#include "tx_types.h"

typedef struct game_ent_def_inst {
    uint32_t id;
    float world_x;
    float world_y;
} game_ent_def_inst;

enum game_tile_flags {
    GAME_TILE_FLAGS_NONE = 0,
    GAME_TILE_FLAGS_FLIP_X = (1 << 0),
    GAME_TILE_FLAGS_FILP_Y = (1 << 1),
};

typedef struct game_tile {
    uint16_t value;
    uint16_t flags;
} game_tile;

typedef enum game_layer_type {
    GAME_LAYER_TYPE_INVALID,
    GAME_LAYER_TYPE_TILES,
    GAME_LAYER_TYPE_ENTITIES,
    GAME_LAYER_TYPE_INTGRID,
    GAME_LAYER_TYPE_COUNT,
} game_layer_type;

typedef enum game_layer_source_type {
    GAME_LAYER_SOURCE_TYPE_INVALID,
    GAME_LAYER_SOURCE_TYPE_TILES,
    GAME_LAYER_SOURCE_TYPE_ENTITIES,
    GAME_LAYER_SOURCE_TYPE_INTGRID,
    GAME_LAYER_SOURCE_TYPE_AUTOLAYER,
    GAME_LAYER_SOURCE_TYPE_COUNT,
} game_layer_source_type;

typedef struct game_layer_inst {
    game_layer_type type;
    uint32_t cell_w;
    uint32_t cell_h;
    uint32_t cell_size;
    game_tile* tiles;
    game_ent_def_inst* ents;
} game_layer_inst;

typedef struct game_level {
    game_layer_inst* layer_insts;
    uint32_t name_id;
} game_level;

typedef struct game_level_proj {
    game_level* levels;
} game_level_proj;

tx_result load_game_level_project(const char* filename, game_level_proj* proj);
tx_result parse_game_level_project(const char* js, size_t len, game_level_proj* proj);
tx_result free_game_level_project(game_level_proj* proj);
