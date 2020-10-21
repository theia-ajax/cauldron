#include "game_level.h"

#include "futils.h"
#include "profile.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSMN_HEADER
#define JSMN_PARENT_LINKS
#include "jsmn.h"

tx_result parse_level(const char* js, jsmntok_t* tokens, int tok_id, game_level* out);
tx_result parse_layer_instance(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_entity_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_tile_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_int_grid_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);
tx_result parse_auto_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out);

game_layer_type js_game_layer_type(const char* js, jsmntok_t token);
bool jseq(const char* js, jsmntok_t token, const char* str);
bool jsstrncpy(const char* js, jsmntok_t token, char* buffer, size_t len);
bool jsisbool(const char* js, jsmntok_t token);
bool jsisnull(const char* js, jsmntok_t token);
bool jsisnum(const char* js, jsmntok_t token);
bool jstod(const char* js, jsmntok_t token, double* out);
bool jstof(const char* js, jsmntok_t token, float* out);
bool jstoi(const char* js, jsmntok_t token, int* out);
bool jstob(const char* js, jsmntok_t token, bool* out);
int jsnextsib(jsmntok_t* tokens, int tok_id);

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

    printf("Loading game level project from '%s' took %llums.", filename, time);

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
                    if (layer->tiles) {
                        arrfree(layer->tiles);
                        memset(layer, 0, sizeof(game_layer_inst));
                    }
                }
                arrfree(level->layer_insts);
                memset(level, 0, sizeof(game_level));
            }
        }
    }
    arrfree(proj->levels);
    memset(proj, 0, sizeof(game_level_proj));
    return TX_SUCCESS;
}

bool jseq(const char* js, jsmntok_t token, const char* str)
{
    return token.type == JSMN_STRING && (int)strlen(str) == token.end - token.start
           && strncmp(js + token.start, str, token.end - token.start) == 0;
}

bool jsstrncpy(const char* js, jsmntok_t token, char* buffer, size_t len)
{
    if (token.type == JSMN_STRING && len > token.end - token.start) {
        strncpy(buffer, js + token.start, token.end - token.start);
        return true;
    }
    return false;
}

bool jsisbool(const char* js, jsmntok_t token)
{
    return token.type == JSMN_PRIMITIVE && (js[token.start] == 't' || js[token.start] == 'f');
}

bool jsisnull(const char* js, jsmntok_t token)
{
    // a primitive that starts with 'n' is always null
    return token.type == JSMN_PRIMITIVE && js[token.start] == 'n';
}

bool jsisnum(const char* js, jsmntok_t token)
{
    return token.type == JSMN_PRIMITIVE && !(jsisbool(js, token) || jsisnull(js, token));
}

bool jstod(const char* js, jsmntok_t token, double* out)
{
    if (jsisnum(js, token)) {
        errno = 0;
        *out = strtod(js + token.start, NULL);
        return errno == 0;
    }
    return false;
}

bool jstof(const char* js, jsmntok_t token, float* out)
{
    if (jsisnum(js, token)) {
        errno = 0;
        *out = strtof(js + token.start, NULL);
        return errno == 0;
    }
    return false;
}

bool jstoi(const char* js, jsmntok_t token, int* out)
{
    double d;
    if (jstod(js, token, &d)) {
        *out = (int)d;
        return true;
    }
    return false;
}

bool jstob(const char* js, jsmntok_t token, bool* out)
{
    if (jsisbool(js, token)) {
        *out = js[token.start] == 't';
        return true;
    }
    return false;
}

// for a given token id find the next sibling token (token with the same parent id).
// The behavior is a little different depending on what tok_id refers to:
//  * key of an object -> the next key in the same object.
//  * value of an object -> the next KEY in the same object.
//  * element of an array -> the next element in the array.
// In any case if no sibling is found at the current depth continue checking at higher depths.
// If no more tokens are found then tok_len (arrlen(tokens)) will be returned.
// tok_id outside of the range [0, tok_len) will yield tok_len
// REQUIRES JSMN_PARENT_LINKS to be defined so that jsmntok_t contains the parent index.
int jsnextsib(jsmntok_t* tokens, int tok_id)
{
    size_t tok_len = arrlen(tokens);
    if (tok_id >= 0 && tok_id < tok_len) {
        const int base_parent = tokens[tok_id].parent;
        for (int cur_id = tok_id + 1; cur_id < tok_len; ++cur_id) {
            if (tokens[cur_id].parent <= base_parent) {
                return cur_id;
            }
        }
    }
    return (int)tok_len;
}

game_layer_type js_game_layer_type(const char* js, jsmntok_t token)
{
    if (jseq(js, token, "Tiles")) {
        return GAME_LAYER_TYPE_TILES;
    } else if (jseq(js, token, "Entities")) {
        return GAME_LAYER_TYPE_ENTITIES;
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

    int found = -1;

    for (int i = tok_id; i < arrlen(tokens); ++i) {
        jsmntok_t token = tokens[i];

        if (jseq(js, token, "layerInstances")) {
            found = i;
            break;
        }
    }

    if (found < 0 || found >= arrlen(tokens) - 1) {
        return TX_INVALID;
    }

    int layer_inst_id = found + 1;
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

    int next = jsnextsib(tokens, tok_id);

    int grid_tiles_id = -1;
    int ents_id = -1;
    for (int i = tok_id + 1; i < next; i = jsnextsib(tokens, i)) {
        jsmntok_t key = tokens[i];
        jsmntok_t val = tokens[i + 1];
        if (jseq(js, key, "__type")) {
            out->type = js_game_layer_type(js, val);
        }
    }

    switch (out->type) {
    case GAME_LAYER_TYPE_TILES:
        return parse_tile_layer(js, tokens, tok_id, out);
    case GAME_LAYER_TYPE_ENTITIES:
        return parse_entity_layer(js, tokens, tok_id, out);
    default:
        return TX_INVALID;
    }
}

tx_result parse_tile_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out)
{
    int next = jsnextsib(tokens, tok_id);

    int grid_tiles_id = -1;
    for (int i = tok_id + 1; i < next; i = jsnextsib(tokens, i)) {
        jsmntok_t key = tokens[i];
        jsmntok_t val = tokens[i + 1];

        if (jseq(js, key, "__cWid")) {
            int w;
            jstoi(js, val, &w);
            out->cell_w = (uint32_t)w;
        } else if (jseq(js, key, "__cHei")) {
            int h;
            jstoi(js, val, &h);
            out->cell_h = (uint32_t)h;
        } else if (jseq(js, key, "__gridSize")) {
            int s;
            jstoi(js, val, &s);
            out->cell_size = (uint32_t)s;
        } else if (grid_tiles_id < 0 && jseq(js, key, "gridTiles")) {
            grid_tiles_id = i + 1;
        }
    }

    if (out->cell_size == 0 || grid_tiles_id < 0) {
        return TX_INVALID;
    }

    size_t tiles_size = out->cell_w * out->cell_h;
    arrsetlen(out->tiles, tiles_size);

    int end = jsnextsib(tokens, grid_tiles_id);
    for (int i = grid_tiles_id + 1; i < end; i = jsnextsib(tokens, i)) {
        jsmntok_t token = tokens[i];
        if (token.type != JSMN_OBJECT) {
            break;
        }
        int next_tile = jsnextsib(tokens, i);
        int coord_id = -1;
        int tile_id = -1;
        for (int j = i + 1; j < next_tile; j = jsnextsib(tokens, j)) {
            jsmntok_t key = tokens[j];
            jsmntok_t value = tokens[j + 1];
            if (coord_id < 0 && jseq(js, key, "coordId")) {
                jstoi(js, value, &coord_id);
            } else if (tile_id < 0 && jseq(js, key, "tileId")) {
                jstoi(js, value, &tile_id);
            }
            if (coord_id >= 0 && tile_id >= 0) {
                break;
            }
        }

        if (coord_id >= 0 && tile_id > 0) {
            printf("tile[%d]=%d\n", coord_id, tile_id);
            out->tiles[coord_id] = (game_tile){
                .value = tile_id,
                .flags = GAME_TILE_FLAGS_NONE,
            };
        }
    }
    return TX_SUCCESS;
}

tx_result parse_entity_layer(const char* js, jsmntok_t* tokens, int tok_id, game_layer_inst* out)
{
    return TX_SUCCESS;
}