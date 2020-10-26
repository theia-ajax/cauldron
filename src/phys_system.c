#include "phys_system.h"

#include "game_level.h"
#include "stb_ds.h"
#include "tx_math.h"

uint8_t* coll_data = NULL;
uint32_t coll_w = 0, coll_h = 0;

float gravity = 50.0f;

void phys_system_init(game_settings* settings)
{
}

void phys_system_shutdown(void)
{
}

void phys_system_load_level(game_level* level)
{
    for (int i = 0; i < arrlen(level->layer_insts); ++i) {
        game_layer_inst* layer = &level->layer_insts[i];
        if (layer->type == GAME_LAYER_TYPE_INTGRID) {
            coll_w = layer->cell_w;
            coll_h = layer->cell_h;
            size_t len = arrlen(layer->tiles);
            arrsetlen(coll_data, len);
            memset(coll_data, 0, sizeof(uint8_t) * len);

            for (size_t j = 0; j < len; ++j) {
                coll_data[j] = (uint8_t)layer->tiles[j].value;
            }

            // just taking the first intgrid layer as the truth of all collision for now.
            break;
        }
    }
}

void phys_system_unload_level(void)
{
    arrfree(coll_data);
    coll_data = NULL;
    coll_w = 0;
    coll_h = 0;
}

void phys_system_update(float dt)
{
}

void phys_system_render(void)
{
}

bool phys_solid(float x, float y, uint16_t layer_mask)
{
    if (x < 0.0f || y < 0.0f || x >= coll_w || y >= coll_h) {
        return false;
    }

    int ix = (int)x;
    int iy = (int)y;

    uint8_t val = coll_data[ix + iy * coll_w];
    return val > 0 && ((1 << (val - 1)) & layer_mask) != 0;
}

float phys_get_gravity()
{
    return gravity;
}