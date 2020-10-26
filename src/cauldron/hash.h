#include "tx_types.h"

// fnv1a constants
#define FNV1A32_OFFSET 2166136261ul
#define FNV1A32_PRIME 16777619ul
#define FNV1A64_OFFSET 14695981039346656037ull
#define FNV1A64_PRIME 1099511628211ull

inline uint32_t hash_append(uint32_t hash, const uint8_t* bytes, size_t len)
{
    TX_ASSERT(bytes);

    for (size_t i = 0; i < len; ++i) {
        hash ^= bytes[i];
        hash *= FNV1A32_PRIME;
    }
    return hash;
}

inline uint32_t hash_data(const uint8_t* bytes, size_t len)
{
    return hash_append(FNV1A32_OFFSET, bytes, len);
}

inline uint32_t hash_string(const char* str)
{
    if (str == NULL) {
        return 0;
    }
    uint32_t hash = FNV1A32_OFFSET;
    size_t i = 0;
    for (size_t i = 0; str[i]; ++i) {
        hash ^= str[i];
        hash *= FNV1A32_PRIME;
    }
    return hash;
}