#include "game_level.h"

#include "futils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSMN_HEADER
#include "jsmn.h"

tx_result parse_level(const char* js, jsmntok_t* tokens, size_t len, int tok_id, game_level* out);

tx_result load_game_data_project(const char* filename, game_data_proj* data)
{
    char* js;
    size_t len;

    tx_result result = read_file_to_buffer(filename, &js, &len);

    if (result != TX_SUCCESS) {
        return result;
    }

    return parse_game_data_project(js, len, data);
}

bool jsoneq(const char* js, jsmntok_t token, const char* str)
{
    return token.type == JSMN_STRING && (int)strlen(str) == token.end - token.start
           && strncmp(js + token.start, str, token.end - token.start) == 0;
}

bool jsontoi(const char* js, jsmntok_t token, int* out)
{
    if (token.type == JSMN_PRIMITIVE) {
        char first = js[token.start];
        if (first == 'f' || first == 't' || first == 'n') {
            return false;
        }
        return sscanf(&js[token.start], "%d", out) > 0;
    }
    return false;
}

tx_result parse_game_data_project(const char* js, size_t len, game_data_proj* data)
{
    tx_result ret = TX_SUCCESS;

    jsmn_parser parser;
    jsmn_init(&parser);

    int tok_count = jsmn_parse(&parser, js, len, NULL, 0);
    jsmntok_t* tokens = malloc(sizeof(jsmntok_t) * tok_count);

    jsmn_init(&parser);
    int tok_parsed = jsmn_parse(&parser, js, len, tokens, tok_count);

    if (tok_parsed < 0) {
        ret = TX_PARSE_ERROR;
        goto exit;
    }

    int levels_find = -1;
    for (int i = 0; i < tok_parsed - 1; ++i) {
        jsmntok_t token = tokens[i];

        if (jsoneq(js, token, "levels") && tokens[i + 1].type == JSMN_ARRAY) {
            levels_find = i + 1;
            break;
        }
    }

    if (levels_find >= 0) {
        jsmntok_t levels_arr_tok = tokens[levels_find];
        data->level_size = levels_arr_tok.size;
        data->levels = (game_level*)malloc(sizeof(game_level) * data->level_size);
        int level_tok_id = levels_find + 1;
        for (int i = 0; i < data->level_size; ++i) {
            parse_level(js, tokens, tok_parsed, level_tok_id, &data->levels[i]);
        }
    }

exit:
    free(tokens);
    return ret;
}

tx_result parse_level(const char* js, jsmntok_t* tokens, size_t len, int tok_id, game_level* out)
{
    int found = -1;

    for (int i = tok_id; i < len; ++i) {
        jsmntok_t token = tokens[i];

        if (jsoneq(js, token, "gridTiles")) {
            found = i;
            break;
        }
    }

    if (found < 0 || found >= len - 1) {
        return TX_INVALID;
    }

    int tiles_id = found + 1;
    jsmntok_t tile_arr_tok = tokens[tiles_id];

    int i = tiles_id + 1;
    int obj_ct = 0;
    while (obj_ct < tile_arr_tok.size) {
        jsmntok_t token = tokens[i];
        if (token.type != JSMN_OBJECT) {
            break;
        }
        ++obj_ct;
        int coord_id = -1;
        int tile_id = -1;
        for (int j = i + 1; j < i + token.size * 2; j += 2) {
            jsmntok_t key = tokens[j];
            jsmntok_t value = tokens[j + 1];
            if (coord_id < 0 && jsoneq(js, key, "coordId")) {
                jsontoi(js, value, &coord_id);
            } else if (tile_id < 0 && jsoneq(js, key, "tileId")) {
                jsontoi(js, value, &tile_id);
            }
            if (coord_id >= 0 && tile_id >= 0) {
                break;
            }
        }

        if (coord_id >= 0 && tile_id > 0) {
            printf("tile[%d]=%d\n", coord_id, tile_id);
            out->tiles[coord_id] = tile_id;
        }

        i += token.size * 2 + 1;
    }
    return TX_SUCCESS;
}