#pragma once

#include "tx_types.h"

typedef struct strhash {
    uint32_t value;
} strhash;

void strhash_init();
void strhash_term();
strhash strhash_get(const char* str);
strhash strhash_get_len(const char* str, int len);
void strhash_discard(strhash str_hash);
const char* strhash_cstr(strhash str_hash);