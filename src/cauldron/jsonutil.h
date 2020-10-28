#pragma once

#define JSMN_HEADER
#define JSMN_PARENT_LINKS
#include "jsmn.h"

#include "tx_types.h"

bool jseq(const char* js, jsmntok_t token, const char* str);
bool jsstrncpy(const char* js, jsmntok_t token, char* buffer, size_t len);
bool jsisbool(const char* js, jsmntok_t token);
bool jsisnull(const char* js, jsmntok_t token);
bool jsisnum(const char* js, jsmntok_t token);
bool jstod(const char* js, jsmntok_t token, double* out);
bool jstof(const char* js, jsmntok_t token, float* out);
bool jstoi(const char* js, jsmntok_t token, int* out);
bool jstob(const char* js, jsmntok_t token, bool* out);
int jstoi_or(const char* js, jsmntok_t token, int def);
bool jstob_or(const char* js, jsmntok_t token, bool def);
int jsnextsib(jsmntok_t* tokens, int tok_id);
jsmntok_t jsget(const char* js, jsmntok_t* tokens, int parent_id, const char* key);
int jsget_id(const char* js, jsmntok_t* tokens, int parent_id, const char* key);
