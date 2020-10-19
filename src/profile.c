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

void profile_start(const char* name)
{
    profile_record def = (profile_record){0};
    hmdefaults(records, def);
    size_t key = stbds_hash_string(name, 42);
    profile_record record = hmgets(records, key);
    record.mark = SDL_GetPerformanceCounter();
    hmputs(records, record);

    mark = SDL_GetPerformanceCounter();
}

uint64_t profile_stop(const char* name)
{
    size_t key = stbds_hash_string(name, 42);
    profile_record record = hmgets(records, key);
    uint64_t delta = SDL_GetPerformanceCounter() - mark;
    record.deltas[record.index] = delta;
    record.index = (record.index + 1) % PROFILE_RING_BUFFER_LEN;
    hmputs(records, record);

    return (delta * 1000) / SDL_GetPerformanceFrequency();
}