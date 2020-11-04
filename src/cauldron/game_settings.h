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
    struct {
        uint32_t level_id;
    } startup;
} game_settings;

game_settings* const get_game_settings();
bool load_game_settings(const char* file_override);
