#include "system_pool.h"
#include "handle.h"
#include "stb_ds.h"
#include "strhash.h"
#include "tx_types.h"

struct anon_pool {
    char* name;
    size_t elem_size;
    void* data;
    uint32_t* handles;
    uint32_t* free_queue;
    uint16_t gen;
};

struct pool_entry {
    strhash key;
    struct anon_pool* pool;
};

struct pool_entry* pools_by_name;

void system_pool_register(char* name, void* pool)
{
    strhash key = strhash_get(name);
    hmputs(
        pools_by_name,
        ((struct pool_entry){
            .key = key,
            .pool = pool,
        }));
}

int pool_usage_count(struct anon_pool* pool)
{
    int count = 0;
    for (int i = 0; i < arrlen(pool->handles); ++i) {
        if (pool->handles[i] != INVALID_RAW_HANDLE) {
            ++count;
        }
    }
    return count;
}

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

void system_pool_editor_window(bool* show)
{
    if (igBegin("Actor Definitions", show, ImGuiWindowFlags_None)) {
        for (int i = 0; i < hmlen(pools_by_name); ++i) {
            struct pool_entry entry = pools_by_name[i];
            igBeginChildStr(entry.pool->name, (ImVec2){0, 100}, true, ImGuiWindowFlags_None);
            igText("%s", entry.pool->name);
            igText("capacity: %d", arrlen(entry.pool->handles));
            igText("usage: %d", pool_usage_count(entry.pool));
            igEndChild();
        }
        igEnd();
    }
}