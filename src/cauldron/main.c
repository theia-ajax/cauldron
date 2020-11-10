
#include "event_system.h"
#include "game_level.h"
#include "game_settings.h"
#include "game_systems.h"
#include "hash.h"
#include "hash_string.h"
#include "player_system.h"
#include "profile.h"
#include "sprite_draw.h"
#include "stb_ds.h"
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
} game_time = {
    .enable_render_interp = true,
    .update_mode = UpdateMode_FixedFrameRate,
    .maximum_updates = 30,
    .update_frequency = 240,
};

int main(int argc, char* argv[])
{
    load_game_settings(NULL);

    game_settings* const settings = get_game_settings();

    game_time.frame_limit = settings->options.video.frame_limit;

    strhash_init();
    txinp_init();
    txrng_seed((uint32_t)time(NULL));

    game_level_proj proj = {0};
    if (load_game_level_project("assets/test_level.json", &proj) != TX_SUCCESS) {
        return 1;
    }

    uint64_t load_ticks = profile_get_last_ticks("load_game_level_project");

    game_systems_init(settings);
    spr_init();

    int index = -1;
    for (int i = 0; i < arrlen(proj.levels); ++i) {
        if (proj.levels[i].name_id == settings->startup.level_id) {
            index = i;
            break;
        }
    }
    if (index >= 0) {
        game_systems_load_level(&proj.levels[index]);
    }

    // unsigned char* text_pixels = NULL;
    // int text_w, text_h;
    // ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &text_pixels, &text_w, &text_h, NULL);

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

        if (txinp_get_key_down(TXINP_KEY_F5)) {
            game_systems_unload_level();
            game_systems_load_level(&proj.levels[0]);
        }

        int update_count = 0;
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

            while (lag >= ms_per_update && update_count < game_time.maximum_updates) {
                game_systems_update(ms_per_update);
                ++update_count;
                lag -= ms_per_update;
                txinp_update();
            }

            // float rt = lag / ms_per_update;
            float rt = (game_time.enable_render_interp) ? lag / ms_per_update : 0.0f;
            game_systems_render(rt);
        } else if (game_time.update_mode == UpdateMode_VariableFrameRate) {
            update_count = 1;
            game_systems_update(dt);
            txinp_update();
            game_systems_render(0.0f);
        }

        // render
        spr_render();

        imgui_begin(dt);
        {
            static bool show_main_menu_bar = false;
            static bool sel_menu_bar_editors_actors = false;
            static bool show_editor_game_time = false;
            static bool show_editor_actors = false;
            static bool show_demo_window = false;

            if (igIsKeyPressed(TXINP_KEY_LALT, false)) {
                show_main_menu_bar = !show_main_menu_bar;
            }

            if (igIsKeyDown(TXINP_KEY_LCTRL) && igIsKeyPressed(TXINP_KEY_A, false)) {
                show_editor_actors = !show_editor_actors;
            }

            if (show_main_menu_bar) {
                if (igBeginMainMenuBar()) {
                    if (igBeginMenu("Editors", true)) {
                        igMenuItemBoolPtr("Actors", "ctrl+a", &show_editor_actors, true);
                        igMenuItemBoolPtr("Game Time", NULL, &show_editor_game_time, true);
                        igEndMenu();
                    }
                    if (igBeginMenu("Misc", true)) {
                        igMenuItemBoolPtr("Demo Window", NULL, &show_demo_window, true);
                        igEndMenu();
                    }
                    igEndMainMenuBar();
                }
            }

            if (show_editor_actors) {
                actor_def_editor_window(&show_editor_actors);
            }

            if (show_editor_game_time) {
                igBegin("Game Time", &show_editor_game_time, ImGuiWindowFlags_NoNavInputs);
                // igText("FPS: %d", dbgui.fps);
                igCheckbox("Enable Render Interp", &game_time.enable_render_interp);
                igInputInt("Frame Limit", &game_time.frame_limit, 1, 10, ImGuiInputTextFlags_None);
                if (igButton(update_mode_names[(int)game_time.update_mode], (ImVec2){0})) {
                    game_time.update_mode =
                        (update_mode)mod((int)game_time.update_mode + 1, (int)UpdateMode_Count);
                }

                if (game_time.update_mode == UpdateMode_FixedFrameRate) {
                    igInputInt(
                        "Update Frequency (Hz)",
                        &game_time.update_frequency,
                        1,
                        10,
                        ImGuiInputTextFlags_None);
                    igInputInt(
                        "Maximum Updates",
                        &game_time.maximum_updates,
                        1,
                        5,
                        ImGuiInputTextFlags_None);
                    igText("Update Count: %d", update_count);
                }

                igEnd();
            }

            if (show_demo_window) {
                igShowDemoWindow(&show_demo_window);
            }
        }
        imgui_end();

        window_swap();
    }

    game_systems_unload_level();

    spr_term();
    game_systems_term();

    free_game_level_project(&proj);

    strhash_term();

    return 0;
}