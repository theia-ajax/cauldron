#pragma once

#include "tx_types.h"

enum {
    GAME_LEVEL_WIDTH = 18,
    GAME_LEVEL_HEIGHT = 32,
};

typedef struct game_layer_instance {
    uint8_t* tiles;
    uint32_t cell_w;
    uint32_t cell_h;
    uint32_t cell_size;
} game_layer_instance;

typedef struct game_level {
    game_layer_instance* layer_insts;
    size_t layer_insts_size;
} game_level;

typedef struct game_level_proj {
    game_level* levels;
    size_t level_size;
} game_level_proj;

tx_result load_game_level_project(const char* filename, game_level_proj* proj);
tx_result parse_game_level_project(const char* js, size_t len, game_level_proj* proj);
tx_result free_game_level_project(game_level_proj* proj);
