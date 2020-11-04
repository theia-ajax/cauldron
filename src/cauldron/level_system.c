#include "level_system.h"
#include "game_level.h"
#include "sprite_draw.h"

game_level* active = NULL;

void level_system_load_level(game_level* level)
{
    active = level;
}

void level_system_unload_level(void)
{
    active = NULL;
}

void level_system_render(float ft)
{
    if (active) {
        for (int lid = 0; lid < arrlen(active->layer_insts); ++lid) {
            game_layer_inst layer = active->layer_insts[lid];
            for (uint32_t x = 0; x < layer.cell_w; x++) {
                for (uint32_t y = 0; y < layer.cell_h; y++) {
                    if (layer.type != GAME_LAYER_TYPE_TILES) {
                        continue;
                    }
                    game_tile tile = layer.tiles[x + y * layer.cell_w];
                    int id = tile.value;
                    sprite_flip flip = SPRITE_FLIP_NONE;
                    if ((tile.flags & GAME_TILE_FLAGS_FLIP_X) != 0) {
                        flip |= SPRITE_FLIP_X;
                    }
                    if ((tile.flags & GAME_TILE_FLAGS_FILP_Y) != 0) {
                        flip |= SPRITE_FLIP_Y;
                    }

                    if (id > 0) {
                        spr_draw(&(sprite_draw_desc){
                            .sprite_id = id,
                            .pos = (vec2){.x = (float)x, .y = (float)y},
                            .flip = flip,
                        });
                    }
                }
            }
        }
    }
}