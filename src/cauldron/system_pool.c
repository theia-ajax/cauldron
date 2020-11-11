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

const uint32_t POOL_SLOT_FREE = 0xFFFFFF00;
const uint32_t POOL_SLOT_USED = 0xFFFF00FF;

void system_pool_editor_window(bool* show)
{
    if (igBegin("System Pools", show, ImGuiWindowFlags_None)) {
        for (int i = 0; i < hmlen(pools_by_name); ++i) {
            struct pool_entry entry = pools_by_name[i];

            int usage = pool_usage_count(entry.pool);
            int capacity = (int)arrlen(entry.pool->handles);
            const float size = 16.0f;
            const float spacing = 2.0f;
            float width = igGetWindowContentRegionWidth();
            int elems_per_row = (int)(width / (size + spacing));
            float height_needed = capacity / elems_per_row * (size + spacing) + 75.0f;

            igBeginChildStr(
                entry.pool->name, (ImVec2){0, height_needed}, true, ImGuiWindowFlags_None);
            {
                igText("%s pool", entry.pool->name);
                igSameLine(0.0f, -1.0f);
                igSeparatorEx(ImGuiSeparatorFlags_Vertical);
                igSameLine(0.0f, -1.0f);
                igText("usage: %d/%d (%0.0f%%)", usage, capacity, usage * 100.0f / capacity);

                igSeparator();

                // ImVec2 pool_vis_size = (ImVec2){.x = igCalcItemWidth(), .y = igGetFrameHeight()};
                ImVec2 pos;
                igGetCursorScreenPos(&pos);

                ImDrawList* draw_list = igGetWindowDrawList();

                width = igGetWindowContentRegionWidth();
                elems_per_row = (int)(width / (size + spacing));

                for (int i = 0; i < arrlen(entry.pool->handles); ++i) {
                    int cx = i % elems_per_row;
                    int cy = i / elems_per_row;

                    ImVec2 p0 = (ImVec2){
                        .x = pos.x + cx * (size + spacing),
                        .y = pos.y + cy * (size + spacing),
                    };
                    ImVec2 p1 = (ImVec2){
                        .x = p0.x + size,
                        .y = p0.y + size,
                    };

                    ImU32 color = (entry.pool->handles[i] == INVALID_RAW_HANDLE) ? POOL_SLOT_FREE
                                                                                 : POOL_SLOT_USED;

                    ImDrawList_AddRectFilled(
                        draw_list, p0, p1, color, 0.0f, ImDrawCornerFlags_None);
                }
            }
            igEndChild();
        }
        igEnd();
    }
}