#include "strhash.h"

#define STRPOOL_U32 uint32_t
#define STRPOOL_U64 uint64_t
#define STRPOOL_IMPLEMENTATION
#include "strpool.h"

strpool_t pool;

void strhash_init()
{
    strpool_init(
        &pool,
        &(strpool_config_t){
            .counter_bits = 24,
            .index_bits = 8,
        });
}

void strhash_term()
{
    strpool_term(&pool);
}

strhash strhash_get(const char* str)
{
    return strhash_get_len(str, (int)strlen(str));
}

strhash strhash_get_len(const char* str, int len)
{
    return (strhash){
        .value = (uint32_t)strpool_inject(&pool, str, len),
    };
}

void strhash_discard(strhash str_hash)
{
    strpool_discard(&pool, (uint64_t)str_hash.value);
}

const char* strhash_cstr(strhash str_hash)
{
    return strpool_cstr(&pool, (uint64_t)str_hash.value);
}
