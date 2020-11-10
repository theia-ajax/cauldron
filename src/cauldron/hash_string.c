#include "hash_string.h"

#define STRPOOL_U32 uint32_t
#define STRPOOL_U64 uint64_t
#define STRPOOL_IMPLEMENTATION
#include "strpool.h"

/**
strpool.h
=========

Highly efficient string pool for C/C++.


Example
-------

    #define  STRPOOL_IMPLEMENTATION
    #include "strpool.h"

    #include <stdio.h> // for printf
    #include <string.h> // for strlen

    int main( int argc, char** argv ) {

        strpool_config_t conf = strpool_default_config;
        //conf.ignore_case = true;

        strpool_t pool;
        strpool_init( &pool, &conf );

        STRPOOL_U64 str_a = strpool_inject( &pool, "This is a test string", (int) strlen( "This is a
test string" ) ); STRPOOL_U64 str_b = strpool_inject( &pool, "THIS IS A TEST STRING", (int) strlen(
"THIS IS A TEST STRING" ) );

        printf( "%s\n", strpool_cstr( &pool, str_a ) );
        printf( "%s\n", strpool_cstr( &pool, str_b ) );
        printf( "%s\n", str_a == str_b ? "Strings are the same" : "Strings are different" );

        strpool_term( &pool );
        return 0;
    }
*/

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
    return (strhash){
        .value = (uint32_t)strpool_inject(&pool, str, (int)strlen(str)),
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
