#include "jsonutil.h"

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

int jstoi_or(const char* js, jsmntok_t token, int def)
{
    int ret = 0;
    if (jstoi(js, token, &ret)) {
        return ret;
    }
    return def;
}

bool jstob_or(const char* js, jsmntok_t token, bool def)
{
    bool ret = false;
    if (jstob(js, token, &ret)) {
        return ret;
    }
    return def;
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

jsmntok_t jsget(const char* js, jsmntok_t* tokens, int parent_id, const char* key)
{
    int id = jsget_id(js, tokens, parent_id, key);
    if (id >= 0) {
        return tokens[id];
    }
    return (jsmntok_t){0};
}

int jsget_id(const char* js, jsmntok_t* tokens, int parent_id, const char* key)
{
    if (parent_id < 0 || parent_id >= arrlen(tokens) || tokens[parent_id].type != JSMN_OBJECT) {
        return -1;
    }

    int next = jsnextsib(tokens, parent_id);
    for (int i = parent_id + 1; i < next; i = jsnextsib(tokens, i)) {
        if (jseq(js, tokens[i], key)) {
            return i + 1;
        }
    }

    return -1;
}
