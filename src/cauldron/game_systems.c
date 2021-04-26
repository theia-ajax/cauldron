#include "game_systems.h"

#include "stb_ds.h"

#include "actor_system.h"
#include "bot_system.h"
#include "entity_system.h"
#include "event_system.h"
#include "level_system.h"
#include "phys_system.h"
#include "player_system.h"
#include "window_system.h"

game_system* g_game_systems;

tx_result game_systems_init(game_settings* settings)
{
    /*
    Descriptions of current game systems

    event_system: Handles an event queue for passing messages between systems. Should be run
    before other systems for consistency of message delivery.

    entity_system: Shortcuts for spawning entities which may have handles to data in multiple
    systems.

    player_system: Directs player input from input sources into game actions.

    bot_system: NPC controllers that produce game actions.

    actor_system: Handles characters that can walk and jump.

    phys_system: Stores physical representation of the world and responds to physics queries.

    level_system: Right now just renders the currently loaded level.

    */

    stbds_arrput(
        g_game_systems,
        ((game_system){
            .name = "event_system",
            .init = event_system_init,
            .term = event_system_term,
            .load_level = NULL,
            .unload_level = NULL,
            .update = event_system_process_queue,
            .render = NULL,
        }));
    // arrput(
    //     g_game_systems,
    //     ((game_system){
    //         .name = "window_system",
    //         .init = window_system_init,
    //         .term = window_system_term,
    //     }));
    // arrput(
    //     g_game_systems,
    //     ((game_system){
    //         .name = "imgui_system",
    //         .init = imgui_system_init,
    //         .term = imgui_system_term,
    //     }));
    arrput(
        g_game_systems,
        ((game_system){
            .name = "entity_system",
            .init = entity_system_init,
            .term = entity_system_term,
            .load_level = entity_system_load_level,
        }));
    arrput(
        g_game_systems,
        ((game_system){
            .name = "player_system",
            .init = player_system_init,
            .term = player_system_term,
            .update = player_system_update,
        }));
    arrput(
        g_game_systems,
        ((game_system){
            .name = "bot_system",
            .init = bot_system_init,
            .term = bot_system_term,
            .unload_level = bot_system_unload_level,
            .update = bot_system_update,
        }));
    arrput(
        g_game_systems,
        ((game_system){
            .name = "actor_system",
            .init = actor_system_init,
            .term = actor_system_term,
            .load_level = NULL,
            .unload_level = actor_system_unload_level,
            .update = actor_system_update,
            .render = actor_system_render,
        }));
    arrput(
        g_game_systems,
        ((game_system){
            .name = "phys_system",
            .init = phys_system_init,
            .term = phys_system_term,
            .load_level = phys_system_load_level,
            .unload_level = phys_system_unload_level,
            .update = phys_system_update,
            .render = NULL,
        }));
    arrput(
        g_game_systems,
        ((game_system){
            .name = "level_system",
            .init = level_system_init,
            .term = level_system_term,
            .load_level = level_system_load_level,
            .unload_level = level_system_unload_level,
            .render = level_system_render,
        }));

    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].init) {
            tx_result result = g_game_systems[i].init(settings);
            if (result != TX_SUCCESS) {
                return result;
            }
        }
    }

    return TX_SUCCESS;
}

void game_systems_term(void)
{
    for (int i = 0; i < arrlen(g_game_systems); ++i) {
        if (g_game_systems[i].term) {
            g_game_systems[i].term();
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
