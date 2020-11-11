#include "game_level.h"

#include "futils.h"
#include "hash.h"
#include "jsonutil.h"
#include "profile.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tx_result parse_level(const char* js, jsmntok_t* tokens, int tok_id, game_level* out);
tx_result parse_layer_instance(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_entity_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_tile_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_int_grid_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_auto_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);

game_layer_source_type js_game_layer_type(const char* js, jsmntok_t token);

tx_result load_game_level_project(const char* filename, game_level_proj* proj)
{
    char* js;
    size_t len;

    profile_start("load_game_level_project");
    tx_result result = read_file_to_buffer(filename, &js, &len);

    if (result != TX_SUCCESS) {
        return result;
    }

    result = parse_game_level_project(js, len, proj);

    uint64_t time = profile_stop("load_game_level_project");

    printf("Loading game level project from '%s' took %llums.\n", filename, time);

    return result;
}

tx_result free_game_level_project(game_level_proj* proj)
{
    if (proj->levels) {
        for (int i = 0; i < arrlen(proj->levels); ++i) {
            game_level* level = &proj->levels[i];
            if (level->layer_insts) {
                for (int j = 0; j < arrlen(level->layer_insts); ++j) {
                    game_layer_inst* layer = &level->layer_insts[j];
                    arrfree(layer->tiles);
                    arrfree(layer->ents);
                }
            }
            arrfree(level->layer_insts);
        }
    }
    arrfree(proj->levels);
    return TX_SUCCESS;
}

game_layer_source_type js_game_layer_type(const char* js, jsmntok_t token)
{
    if (jseq(js, token, "Tiles")) {
        return GAME_LAYER_SOURCE_TYPE_TILES;
    } else if (jseq(js, token, "Entities")) {
        return GAME_LAYER_SOURCE_TYPE_ENTITIES;
    } else if (jseq(js, token, "IntGrid")) {
        return GAME_LAYER_SOURCE_TYPE_INTGRID;
    } else if (jseq(js, token, "AutoLayer")) {
        return GAME_LAYER_SOURCE_TYPE_AUTOLAYER;
    } else {
        return GAME_LAYER_TYPE_INVALID;
    }
}

tx_result parse_game_level_project(const char* js, size_t len, game_level_proj* proj)
{
    // to keep ourselves sane we will just memset every structure to zero throughout the whole tree
    // of data so that it's easier to identify which pointers need to be freed
    memset(proj, 0, sizeof(game_level_proj));

    tx_result ret = TX_SUCCESS;

    jsmn_parser parser;
    jsmn_init(&parser);

    int tok_needed = jsmn_parse(&parser, js, len, NULL, 0);
    jsmntok_t* tokens = NULL;
    arrsetlen(tokens, tok_needed);

    jsmn_init(&parser);
    int tok_len = jsmn_parse(&parser, js, len, tokens, tok_needed);

    if (tok_len < 0) {
        ret = TX_PARSE_ERROR;
        goto exit;
    }

    int levels_find = -1;
    for (int i = 1; i < tok_len; i = jsnextsib(tokens, i)) {
        jsmntok_t token = tokens[i];

        if (jseq(js, token, "levels") && tokens[i + 1].type == JSMN_ARRAY) {
            levels_find = i + 1;
            break;
        }
    }

    if (levels_find >= 0) {
        size_t levels_len = tokens[levels_find].size;
        arrsetlen(proj->levels, levels_len);
        int level_tok_id = levels_find + 1;
        for (int i = 0; i < levels_len; ++i) {
            parse_level(js, tokens, level_tok_id, &proj->levels[i]);
            level_tok_id = jsnextsib(tokens, level_tok_id);
        }
    }

exit:
    arrfree(tokens);
    return ret;
}

tx_result parse_level(const char* js, jsmntok_t* tokens, int tok_id, game_level* out)
{
    memset(out, 0, sizeof(game_level));

    jsmntok_t name_tok = jsget(js, tokens, tok_id, "identifier");
    out->name_id = strhash_get_len(js + name_tok.start, name_tok.end - name_tok.start);

    int layer_inst_id = jsget_id(js, tokens, tok_id, "layerInstances");

    if (layer_inst_id < 0) {
        return TX_INVALID;
    }

    jsmntok_t layer_inst_tok = tokens[layer_inst_id];

    if (layer_inst_tok.type != JSMN_ARRAY) {
        return TX_INVALID;
    }

    size_t layer_insts_size = layer_inst_tok.size;
    arrsetlen(out->layer_insts, layer_insts_size);

    int layer_tok_id = layer_inst_id + 1;
    for (size_t i = 0; i < layer_insts_size; ++i) {
        parse_layer_instance(js, tokens, layer_tok_id, &out->layer_insts[i]);
        layer_tok_id = jsnextsib(tokens, layer_tok_id);
    }

    return TX_SUCCESS;
}

tx_result parse_layer_instance(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out)
{
    memset(out, 0, sizeof(game_layer_inst));

    game_layer_source_type sourceType = js_game_layer_type(js, jsget(js, tokens, tok_id, "__type"));

    switch (sourceType) {
    case GAME_LAYER_SOURCE_TYPE_TILES:
        printf("Parsing tile layer...\n");
        return parse_tile_layer(js, tokens, tok_id, out);
    case GAME_LAYER_SOURCE_TYPE_INTGRID:
        printf("Parsing intgrid layer...\n");
        return parse_int_grid_layer(js, tokens, tok_id, out);
    case GAME_LAYER_SOURCE_TYPE_ENTITIES:
        printf("Parsing entity layer...\n");
        return parse_entity_layer(js, tokens, tok_id, out);
    case GAME_LAYER_SOURCE_TYPE_AUTOLAYER:
        printf("Parsing autolayer layer...\n");
        return parse_auto_layer(js, tokens, tok_id, out);
    default:
        return TX_INVALID;
    }
}

tx_result parse_tile_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out)
{
    out->type = GAME_LAYER_TYPE_TILES;
    out->cell_w = jstoi_or(js, jsget(js, tokens, tok_id, "__cWid"), 0);
    out->cell_h = jstoi_or(js, jsget(js, tokens, tok_id, "__cHei"), 0);
    out->cell_size = jstoi_or(js, jsget(js, tokens, tok_id, "__gridSize"), 0);
    int grid_tiles_id = jsget_id(js, tokens, tok_id, "gridTiles");

    if (out->cell_size == 0 || grid_tiles_id < 0) {
        return TX_INVALID;
    }

    size_t tiles_size = out->cell_w * out->cell_h;
    arrsetlen(out->tiles, tiles_size);
    memset(out->tiles, 0, sizeof(game_tile) * tiles_size);

    int end = jsnextsib(tokens, grid_tiles_id);
    for (int i = grid_tiles_id + 1; i < end; i = jsnextsib(tokens, i)) {
        if (tokens[i].type != JSMN_OBJECT) {
            break;
        }

        int coord_id = jstoi_or(js, jsget(js, tokens, i, "coordId"), -1);
        int tile_id = jstoi_or(js, jsget(js, tokens, i, "tileId"), 0);

        if (coord_id >= 0 && tile_id > 0) {
            // printf("tile[%d]=%d\n", coord_id, tile_id);
            out->tiles[coord_id] = (game_tile){
                .value = (uint16_t)tile_id,
                .flags = GAME_TILE_FLAGS_NONE,
            };
        }
    }
    return TX_SUCCESS;
}

tx_result parse_int_grid_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out)
{
    out->type = GAME_LAYER_TYPE_INTGRID;
    out->cell_w = jstoi_or(js, jsget(js, tokens, tok_id, "__cWid"), 0);
    out->cell_h = jstoi_or(js, jsget(js, tokens, tok_id, "__cHei"), 0);
    out->cell_size = jstoi_or(js, jsget(js, tokens, tok_id, "__gridSize"), 0);
    int int_grid_id = jsget_id(js, tokens, tok_id, "intGrid");

    if (out->cell_size == 0 || int_grid_id < 0) {
        return TX_INVALID;
    }

    size_t tiles_size = out->cell_w * out->cell_h;
    arrsetlen(out->tiles, tiles_size);
    memset(out->tiles, 0, sizeof(game_tile) * tiles_size);

    int end = jsnextsib(tokens, int_grid_id);
    for (int i = int_grid_id + 1; i < end; i = jsnextsib(tokens, i)) {
        if (tokens[i].type != JSMN_OBJECT) {
            break;
        }

        int coord_id = jstoi_or(js, jsget(js, tokens, i, "coordId"), -1);
        int val = jstoi_or(js, jsget(js, tokens, i, "v"), 0);

        if (coord_id >= 0 && val > 0) {
            // printf("int_grid[%d]=%d\n", coord_id, val);
            out->tiles[coord_id] = (game_tile){
                .value = (uint16_t)val,
                .flags = GAME_TILE_FLAGS_NONE,
            };
        }
    }
    return TX_SUCCESS;
}

tx_result parse_auto_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out)
{
    out->type = GAME_LAYER_TYPE_TILES;
    out->cell_w = jstoi_or(js, jsget(js, tokens, tok_id, "__cWid"), 0);
    out->cell_h = jstoi_or(js, jsget(js, tokens, tok_id, "__cHei"), 0);
    out->cell_size = jstoi_or(js, jsget(js, tokens, tok_id, "__gridSize"), 0);
    int auto_layer_id = jsget_id(js, tokens, tok_id, "autoLayerTiles");

    if (out->cell_size == 0 || auto_layer_id < 0) {
        return TX_INVALID;
    }

    size_t tiles_size = out->cell_w * out->cell_h;
    arrsetlen(out->tiles, tiles_size);
    memset(out->tiles, 0, sizeof(game_tile) * tiles_size);

    int end = jsnextsib(tokens, auto_layer_id);
    int next = end;
    for (int i = auto_layer_id + 1; i < end; i = next) {
        if (tokens[i].type != JSMN_OBJECT) {
            break;
        }

        next = jsnextsib(tokens, i);

        int px_id = jsget_id(js, tokens, i, "px");
        int src_id = jsget_id(js, tokens, i, "src");
        int flips = jstoi_or(js, jsget(js, tokens, i, "f"), 0);
        int data_id = jsget_id(js, tokens, i, "d");
        int coord_id, tile_id;
        jstoi(js, tokens[data_id + 2], &coord_id);
        jstoi(js, tokens[data_id + 3], &tile_id);

        if (coord_id >= 0 && tile_id > 0 /* && out->tiles[coord_id].value == 0*/) {
            // printf("auto_layer[%d]=%d\n", coord_id, tile_id);
            out->tiles[coord_id] = (game_tile){
                .value = (uint16_t)tile_id,
                .flags = flips,
            };
        }
    }
    return TX_SUCCESS;
}

tx_result parse_entity_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out)
{
    out->type = GAME_LAYER_TYPE_ENTITIES;
    int ent_insts_id = jsget_id(js, tokens, tok_id, "entityInstances");

    if (ent_insts_id < 0) {
        return TX_INVALID;
    }

    int ent_count = tokens[ent_insts_id].size;
    arrsetlen(out->ents, ent_count);
    memset(out->ents, 0, sizeof(game_ent_def_inst) * ent_count);

    int index = 0;
    int end = jsnextsib(tokens, ent_insts_id);
    for (int i = ent_insts_id + 1; i < end; i = jsnextsib(tokens, i)) {
        if (tokens[i].type != JSMN_OBJECT) {
            break;
        }

        jsmntok_t id_tok = jsget(js, tokens, i, "__identifier");
        if (id_tok.type == JSMN_STRING) {
            out->ents[index].id = strhash_get_len(js + id_tok.start, id_tok.end - id_tok.start);
        } else {
            out->ents[index].id = (strhash){0};
        }

        float world_x_px, world_y_px;

        int px_id = jsget_id(js, tokens, i, "px");
        jstof(js, tokens[px_id + 1], &world_x_px);
        jstof(js, tokens[px_id + 2], &world_y_px);

        out->ents[index].world_x = world_x_px / 8.0f;
        out->ents[index].world_y = world_y_px / 8.0f;

        printf(
            "Entity = { id = %s, x = %0.0f, y = %0.0f }\n",
            strhash_cstr(out->ents[index].id),
            out->ents[index].world_x,
            out->ents[index].world_y);

        index++;
    }

    return TX_SUCCESS;
}