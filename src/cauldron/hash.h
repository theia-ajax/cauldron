#pragma once

#include <xxhash.h>

enum { HASH_SEED = 0x0badf00d };

#define hash_data(bytes, len) XXH32(bytes, len, HASH_SEED)
#define hash_string(str) XXH32(str, strlen(str), HASH_SEED)