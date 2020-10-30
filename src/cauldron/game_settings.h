#pragma once

#include "tx_types.h"

typedef struct game_settings {
    struct {
        struct {
            int display_width;
            int display_height;
            bool enable_vsync;
            int frame_limit;
        } video;
    } options;
} game_settings;

game_settings* const get_game_settings();
bool load_game_settings(const char* file_override);
