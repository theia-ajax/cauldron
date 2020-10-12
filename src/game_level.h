#pragma once

#include "tx_types.h"

enum {
    GAME_LEVEL_WIDTH = 18,
    GAME_LEVEL_HEIGHT = 32,
};

typedef struct game_level {
    uint8_t tiles[GAME_LEVEL_WIDTH * GAME_LEVEL_HEIGHT];
} game_level;

typedef struct game_data_proj {
    game_level* levels;
    size_t level_size;
} game_data_proj;

tx_result load_game_data_project(const char* filename, game_data_proj* data);
tx_result parse_game_data_project(const char* js, size_t len, game_data_proj* data);
