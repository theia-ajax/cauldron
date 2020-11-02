#include "game_systems.h"

#include "stb_ds.h"

#include "actor_system.h"
#include "bot_system.h"
#include "entity_system.h"
#include "phys_system.h"
#include "player_system.h"

#define GAME_SYS_FUNC(sys, func) sys##_system_##func

#define GAME_SYSTEM(sys)                                                                           \
    ((game_system){                                                                                \
        .name = #sys "_system",                                                                    \
        .init = GAME_SYS_FUNC(sys, init),                                                          \
        .shutdown = GAME_SYS_FUNC(sys, shutdown),                                                  \
        .load_level = GAME_SYS_FUNC(sys, load_level),                                              \
        .unload_level = GAME_SYS_FUNC(sys, unload_level),                                          \
        .update = GAME_SYS_FUNC(sys, update),                                                      \
        .render = GAME_SYS_FUNC(sys, render),                                                      \
        .config_ui = NULL,                                                                         \
        .debug_ui = NULL,                                                                          \
    })

#define GAME_SYSTEM_DEBUG(sys)                                                                     \
    ((game_system){                                                                                \
        .name = #sys "_system",                                                                    \
        .init = GAME_SYS_FUNC(sys, init),                                                          \
        .shutdown = GAME_SYS_FUNC(sys, shutdown),                                                  \
        .load_level = GAME_SYS_FUNC(sys, load_level),                                              \
        .unload_level = GAME_SYS_FUNC(sys, unload_level),                                          \
        .update = GAME_SYS_FUNC(sys, update),                                                      \
        .render = GAME_SYS_FUNC(sys, render),                                                      \
        .config_ui = GAME_SYS_FUNC(sys, config_ui),                                                \
        .debug_ui = GAME_SYS_FUNC(sys, debug_ui),                                                  \
    })

game_system* g_game_systems;

void game_systems_init(game_settings* settings)
{
    arrput(g_game_systems, GAME_SYSTEM(entity));
    arrput(g_game_systems, GAME_SYSTEM(player));
    arrput(g_game_systems, GAME_SYSTEM(bot));
    arrput(g_game_systems, GAME_SYSTEM_DEBUG(actor));
    arrput(g_game_systems, GAME_SYSTEM(phys));

    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].init) {
            g_game_systems[i].init(settings);
        }
    }
}

void game_systems_shutdown(void)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].shutdown) {
            g_game_systems[i].shutdown();
        }
    }
}

void game_systems_load_level(game_level* level)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].load_level) {
            g_game_systems[i].load_level(level);
        }
    }
}

void game_systems_unload_level(void)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].unload_level) {
            g_game_systems[i].unload_level();
        }
    }
}

void game_systems_update(float dt)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].update) {
            g_game_systems[i].update(dt);
        }
    }
}

void game_systems_render(float rt)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].render) {
            g_game_systems[i].render(rt);
        }
    }
}

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

void game_systems_config_ui(void)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].config_ui) {
            if (igCollapsingHeaderTreeNodeFlags(g_game_systems[i].name, ImGuiTreeNodeFlags_None)) {
                g_game_systems[i].config_ui();
            }
        }
    }
}

void game_systems_debug_ui(void)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].debug_ui) {
            g_game_systems[i].debug_ui();
        }
    }
}
