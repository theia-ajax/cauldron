#include "editor_windows.h"
#include "event_system.h"
#include "game_level.h"
#include "game_settings.h"
#include "game_systems.h"
#include "level_system.h"
#include "player_system.h"
#include "profile.h"
#include "sprite_draw.h"
#include "stb_ds.h"
#include "system_pool.h"
#include "tx_input.h"
#include "tx_math.h"
#include "tx_rand.h"
#include "tx_types.h"
#include "window_system.h"
#include <GL/gl3w.h>
#include <SDL2/SDL.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "flecs.h"

#include "system_sdl2.h"
#include "system_window_sdl2.h"

typedef enum update_mode {
    UpdateMode_VariableFrameRate,
    UpdateMode_FixedFrameRate,
    UpdateMode_Count,
} update_mode;

char* update_mode_names[UpdateMode_Count] = {"Variable", "Fixed"};

struct game_time_desc {
    bool enable_render_interp;
    int frame_limit;
    int update_frequency;
    update_mode update_mode;
    int maximum_updates;
    int curr_fps;
    int update_count;
} game_time = {
    .enable_render_interp = true,
    .update_mode = UpdateMode_FixedFrameRate,
    .maximum_updates = 30,
    .update_frequency = 144,
};
void game_time_editor_window(bool*);

typedef vec2 Position;
typedef vec2 Velocity;
typedef struct {
    uint32_t sprite_id;
    float layer;
    vec2 origin;
    sprite_flip flip;
} Sprite;
typedef uint32_t PlayerControlId;
typedef uint32_t Sdl2Input;

void actor_move(ecs_iter_t* it)
{
    Position* p = ecs_column(it, Position, 1);
    Velocity* v = ecs_column(it, Velocity, 2);

    for (int i = 0; i < it->count; ++i) {
        *p = vec2_add(*p, vec2_scale(*v, it->delta_time));
    }
}

void actor_render(ecs_iter_t* it)
{
    Position* pos = ecs_column(it, Position, 1);
    Sprite* spr = ecs_column(it, Sprite, 2);

    for (int i = 0; i < it->count; ++i) {
        spr_draw(&(sprite_draw_desc){
            .sprite_id = spr->sprite_id,
            .pos = *pos,
            .origin = spr->origin,
            .layer = spr->layer,
            .flip = spr->flip,
        });
    }
}

void sdl2_process_input(ecs_iter_t* it)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            ecs_quit(it->world);
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                ecs_quit(it->world);
            }
            txinp_on_key_event((txinp_event_key){
                .is_down = true,
                .key = event.key.keysym.scancode,
            });
            break;

        case SDL_KEYUP:
            txinp_on_key_event((txinp_event_key){
                .is_down = false,
                .key = event.key.keysym.scancode,
            });
            break;
        }
    }

    txinp_update();
}

int main(int argc, char* argv[])
{
    strhash_init();
    load_game_settings(NULL);
    game_settings* const settings = get_game_settings();

    ecs_world_t* world = ecs_init_w_args(argc, argv);
    ecs_set_target_fps(world, 144.0f);

    ECS_IMPORT(world, SystemSdl2);
    ECS_IMPORT(world, SystemSdl2Window);

    ecs_entity_t window =
        ecs_set(world, 0, WindowDesc, {.title = "cauldron", .width = 1280, .height = 720});

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Sprite);
    ECS_COMPONENT(world, PlayerControlId);

    ECS_SYSTEM(world, actor_move, EcsPreUpdate, Position, Velocity);
    ECS_SYSTEM(world, actor_render, EcsOnStore, Position, Sprite);

    ECS_ENTITY(world, MyEnt, Position, Velocity, Sprite);
    ecs_set(world, MyEnt, Position, {.x = 0, .y = 2});
    ecs_set(world, MyEnt, Velocity, {.x = 1.0f});
    ecs_set(
        world, MyEnt, Sprite, {.sprite_id = 1, .origin = {0.5f, 0.5f}, .layer = -5.0f, .flip = 0});

    spr_init();

    while (ecs_progress(world, 0.0f)) {

        spr_render();
    }

    spr_term();

    return ecs_fini(world);

    txrng_seed((uint32_t)time(NULL));

    game_time.frame_limit = settings->options.video.frame_limit;

    txinp_init();
    game_systems_init(settings);
    spr_init();

    editor_windows_init(&(editor_windows_sys_desc){
        .windows = {
            [0] =
                {
                    .menu_path = "Game/Actor Defs",
                    .window_proc = actor_def_editor_window,
                    .shortcut.mod = TXINP_MOD_CTRL,
                    .shortcut.key = TXINP_KEY_A,
                },
            [1] =
                {
                    .menu_path = "Game/Editors/Game Time",
                    .window_proc = game_time_editor_window,
                    .shortcut.mod = TXINP_MOD_CTRL,
                    .shortcut.key = TXINP_KEY_T,
                },
            [2] =
                {
                    .menu_path = "Game/Levels",
                    .window_proc = level_select_window,
                    .shortcut.mod = TXINP_MOD_CTRL,
                    .shortcut.key = TXINP_KEY_L,
                },
            [3] =
                {
                    .menu_path = "Debug/System Pools",
                    .window_proc = system_pool_editor_window,
                    .shortcut.mod = TXINP_MOD_CTRL,
                    .shortcut.key = TXINP_KEY_P,
                },
            [4] =
                {
                    .menu_path = "Misc/Demo Window",
                    .window_proc = igShowDemoWindow,
                },
        }});

    level_load_id(settings->startup.level_id);

    uint64_t last_ticks = SDL_GetPerformanceCounter();
    float time = 0.0f;
    float next_sec = time + 1.0f;
    float lag = 0.0f;
    bool is_running = true;
    int frames_this_sec = 0;
    while (is_running) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    is_running = false;
                }

                txinp_on_key_event((txinp_event_key){
                    .key = (txinp_key)event.key.keysym.scancode,
                    .is_down = true,
                });
                break;

            case SDL_KEYUP:
                txinp_on_key_event((txinp_event_key){
                    .key = (txinp_key)event.key.keysym.scancode,
                    .is_down = false,
                });
                break;
            }
        }

        game_time.update_count = 0;
        uint64_t frequency = get_frequency();
        uint64_t ticks = get_ticks();
        uint64_t delta_ticks = ticks - last_ticks;
        last_ticks = ticks;

        int frame_limit = game_time.frame_limit;
        if (frame_limit > 0) {
            uint64_t min_frame_delta_ticks = frequency / frame_limit;
            uint64_t over_ticks = 0;

            while (delta_ticks < min_frame_delta_ticks) {
                ticks = get_ticks();
                delta_ticks = ticks - last_ticks;
            }
            over_ticks = delta_ticks - min_frame_delta_ticks;
            last_ticks = ticks - over_ticks;
            delta_ticks -= over_ticks;
        }

        ++frames_this_sec;
        if (time >= next_sec) {
            next_sec += 1.0f;
            game_time.curr_fps = frames_this_sec;
            frames_this_sec = 0;
        }

        float dt = (float)delta_ticks / frequency;
        time += dt;

        // time
        if (game_time.update_mode == UpdateMode_FixedFrameRate) {
            lag += dt;

            game_time.update_frequency = min(max(game_time.update_frequency, 1), 500);
            const float ms_per_update = 1.0f / game_time.update_frequency;

            while (lag >= ms_per_update && game_time.update_count < game_time.maximum_updates) {
                game_systems_update(ms_per_update);
                ++game_time.update_count;
                lag -= ms_per_update;
                txinp_update();
            }

            // float rt = lag / ms_per_update;
            float rt = (game_time.enable_render_interp) ? lag / ms_per_update : 0.0f;
            game_systems_render(rt);
        } else if (game_time.update_mode == UpdateMode_VariableFrameRate) {
            game_time.update_count = 1;
            game_systems_update(dt);
            txinp_update();
            game_systems_render(0.0f);
        }

        // render
        spr_render();

        // imgui_begin(dt);
        // {
        //     editor_windows_process_shortcuts();

        //     static bool show_main_menu_bar = false;
        //     if (igIsKeyPressed(TXINP_KEY_LALT, true)) {
        //         show_main_menu_bar = !show_main_menu_bar;
        //     }

        //     if (show_main_menu_bar) {
        //         editor_windows_draw_menu_bar();
        //     }

        //     editor_windows_draw_windows();
        // }
        // imgui_end();

        // window_swap();
    }

    game_systems_unload_level();

    editor_windows_term();

    spr_term();
    game_systems_term();

    strhash_term();

    return 0;
}

void game_time_editor_window(bool* open)
{
    igBegin("Game Time", open, ImGuiWindowFlags_NoNavInputs);
    igText("FPS: %d", game_time.curr_fps);
    igCheckbox("Enable Render Interp", &game_time.enable_render_interp);
    igInputInt("Frame Limit", &game_time.frame_limit, 1, 10, ImGuiInputTextFlags_None);
    if (igButton(update_mode_names[(int)game_time.update_mode], (ImVec2){0})) {
        game_time.update_mode =
            (update_mode)mod((int)game_time.update_mode + 1, (int)UpdateMode_Count);
    }

    if (game_time.update_mode == UpdateMode_FixedFrameRate) {
        igInputInt(
            "Update Frequency (Hz)", &game_time.update_frequency, 1, 10, ImGuiInputTextFlags_None);
        igInputInt("Maximum Updates", &game_time.maximum_updates, 1, 5, ImGuiInputTextFlags_None);
        igText("Update Count: %d", game_time.update_count);
    }

    igEnd();
}