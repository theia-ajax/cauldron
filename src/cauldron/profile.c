#include "profile.h"

#include "tx_types.h"
#include <SDL2/SDL.h>

#define PROFILE_RING_BUFFER_LEN 100

typedef struct profile_report {
    uint64_t mark;
    uint64_t deltas[PROFILE_RING_BUFFER_LEN];
    uint32_t index;
} profile_report;

typedef struct profile_record {
    size_t key;
    uint64_t mark;
    uint64_t deltas[PROFILE_RING_BUFFER_LEN];
    uint32_t index;
} profile_record;

profile_record* records = NULL;

uint64_t mark;

void profile_start(char* name)
{
    profile_record def = (profile_record){0};
    hmdefaults(records, def);
    size_t key = stbds_hash_string(name, 42);
    profile_record record = hmgets(records, key);
    record.mark = SDL_GetPerformanceCounter();
    record.key = key;
    hmputs(records, record);

    mark = SDL_GetPerformanceCounter();
}

uint64_t profile_stop(char* name)
{
    size_t key = stbds_hash_string(name, 42);
    profile_record record = hmgets(records, key);
    uint64_t delta = SDL_GetPerformanceCounter() - mark;
    record.index = (record.index + 1) % PROFILE_RING_BUFFER_LEN;
    record.deltas[record.index] = delta;
    hmputs(records, record);

    return (delta * 1000) / SDL_GetPerformanceFrequency();
}

uint64_t profile_get_last_ticks(char* name)
{
    size_t key = stbds_hash_string(name, 42);
    if (hmgeti(records, key) >= 0) {
        profile_record* r = &hmgets(records, key);
        return r->deltas[r->index];
    }
    return 0;
}

uint64_t get_ticks()
{
    return SDL_GetPerformanceCounter();
}

uint64_t get_frequency()
{
    return SDL_GetPerformanceFrequency();
}