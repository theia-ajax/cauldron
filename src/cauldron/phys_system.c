#include "phys_system.h"

#include "game_level.h"
#include "stb_ds.h"
#include "tx_math.h"

typedef struct tile_phys {
    uint16_t layer;
    phys_tile_shape shape;
} tile_phys;

typedef struct tile_shapes {
    uint32_t key;
    phys_tile_shape shape;
} tile_phys_ref;

enum flip { FLIP_NONE, FLIP_X, FLIP_Y };

inline uint32_t tile_flip_key(uint16_t value, enum flip flip)
{
    return (uint32_t)(value + ((uint32_t)flip << 16));
}

tile_phys* tiles = NULL;
tile_phys_ref* tile_phys_map = NULL;
uint32_t tiles_w = 0, tiles_h = 0;
float gravity = 50.0f;

void phys_system_init(game_settings* settings)
{
    for (int flip = 0; flip < 4; ++flip) {
        hmputs(
            tile_phys_map,
            ((tile_phys_ref){
                .key = tile_flip_key(61, flip),
                .shape =
                    (phys_tile_shape){
                        .type = PHYS_TILE_SHAPE_SLOPE,
                        .flips = flip,
                    },
            }));
    }

    hmputs(
        tile_phys_map,
        ((tile_phys_ref){
            .key = tile_flip_key(60, 0),
            .shape = PHYS_TILE_SHAPE_PLATFORM,
        }));
}

void phys_system_shutdown(void)
{
    hmfree(tile_phys_map);
}

void phys_system_load_level(game_level* level)
{
    game_layer_inst* coll_layer = NULL;
    game_layer_inst* tile_layer = NULL;
    for (int i = 0; i < arrlen(level->layer_insts); ++i) {
        game_layer_inst* layer = &level->layer_insts[i];
        if (!coll_layer && layer->type == GAME_LAYER_TYPE_INTGRID) {
            coll_layer = layer;
        }
        if (!tile_layer && layer->type == GAME_LAYER_TYPE_TILES) {
            tile_layer = layer;
        }
    }

    tiles_w = coll_layer->cell_w;
    tiles_h = coll_layer->cell_h;
    size_t len = arrlen(coll_layer->tiles);
    arrsetlen(tiles, len);
    memset(tiles, 0, sizeof(tile_phys) * len);

    for (size_t i = 0; i < len; ++i) {
        game_tile* coll_tile = &coll_layer->tiles[i];
        uint16_t layer = coll_tile->value;
        uint16_t flips = tile_layer->tiles[i].flags & 0x3;
        uint32_t key = tile_flip_key(tile_layer->tiles[i].value, flips);
        tile_phys_ref phys_ref = stbds_hmgets(tile_phys_map, key);
        tiles[i] = (tile_phys){
            .layer = layer,
            .shape = phys_ref.shape,
        };
    }
}

void phys_system_unload_level(void)
{
    arrfree(tiles);
    tiles_w = 0;
    tiles_h = 0;
}

void phys_system_update(float dt)
{
}

void phys_system_render(void)
{
}

bool _phys_shape_solid(phys_tile_shape shape, float nx, float ny)
{
    switch (shape.type) {
    default:
    case PHYS_TILE_SHAPE_FULL_TILE:
        return true;
    case PHYS_TILE_SHAPE_PLATFORM:
        return ny <= 0.3755f;
    case PHYS_TILE_SHAPE_WALL:
        return nx >= 0.375f && nx < 0.625f;
    case PHYS_TILE_SHAPE_SLOPE:
        switch (shape.flips) {
        default:
        case 0:
            return nx >= (1.0f - ny);
        case 1:
            return nx <= ny;
        case 2:
            return nx >= ny;
        case 3:
            return nx <= (1.0f - ny);
        }
    }
}

bool phys_solid(float x, float y, uint16_t layer_mask)
{
    if (x < 0.0f || y < 0.0f || x >= tiles_w || y >= tiles_h) {
        return false;
    }

    int ix = (int)x;
    int iy = (int)y;

    tile_phys* tp = &tiles[ix + iy * tiles_w];
    uint16_t layer = tp->layer;
    return layer > 0 && ((1 << (layer - 1)) & layer_mask) != 0
           && _phys_shape_solid(tp->shape, x - ix, y - iy);
}

float phys_get_gravity()
{
    return gravity;
}