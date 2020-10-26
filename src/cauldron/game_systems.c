#include "game_systems.h"

#include "stb_ds.h"

#include "actor_system.h"
#include "phys_system.h"
#include "player_system.h"

#define GAME_SYSTEM_FUNC_NAME(sys, func) sys##_system_##func

#define GAME_SYSTEM(sys)                                                                           \
    (game_system)                                                                                  \
    {                                                                                              \
        .init = GAME_SYSTEM_FUNC_NAME(sys, init),                                                  \
        .shutdown = GAME_SYSTEM_FUNC_NAME(sys, shutdown),                                          \
        .load_level = GAME_SYSTEM_FUNC_NAME(sys, load_level),                                      \
        .unload_level = GAME_SYSTEM_FUNC_NAME(sys, unload_level),                                  \
        .update = GAME_SYSTEM_FUNC_NAME(sys, update),                                              \
        .render = GAME_SYSTEM_FUNC_NAME(sys, render),                                              \
    }

game_system* g_game_systems;

void game_systems_init(game_settings* settings)
{
    arrput(g_game_systems, GAME_SYSTEM(player));
    arrput(g_game_systems, GAME_SYSTEM(actor));
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

void game_systems_render(void)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].render) {
            g_game_systems[i].render();
        }
    }
}