
#include "sokol_gfx.h"
#include "sprite_draw.h"
#include "stb_ds.h"
#include "tinyobj_loader_c.h"
#include "tx_math.h"
#include "tx_types.h"
#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define TX_INPUT_IMPLEMENTATION
#include "tx_input.h"

void read_file_to_buffer(const char* filename, char** buffer, size_t* len);

int main(int argc, char* argv[])
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "cauldron",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    TX_ASSERT(gl3wInit() == GL3W_OK);

    sg_setup(&(sg_desc){0});
    spr_init();
    txinp_init();

    sg_pass_action pass_action = {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.0f, 0.0f, 0.0f},
        }};

    uint64_t last_ticks = SDL_GetPerformanceCounter();
    float time = 0.0f;
    bool is_running = true;
    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
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

        // time
        uint64_t ticks = SDL_GetPerformanceCounter();
        uint64_t delta_ticks = ticks - last_ticks;
        last_ticks = ticks;
        uint64_t frequency = SDL_GetPerformanceFrequency();
        float dt = (float)delta_ticks / frequency;
        time += dt;

        // update
        if (txinp_get_key(TXINP_KEY_UP)) {
        }

        if (txinp_get_key(TXINP_KEY_RIGHT)) {
        }

        if (txinp_get_key(TXINP_KEY_LEFT)) {
        }

        for (float x = -8.0f; x < 8.0f; x += 0.25f) {
            spr_draw((vec3){.x = x * 1.0f, .y = sinf(time + x) * 8.0f});
        }

        // spr_draw((vec3){0, 0, 0});

        // render
        int cur_width, cur_height;
        SDL_GetWindowSize(window, &cur_width, &cur_height);
        sg_begin_default_pass(&pass_action, cur_width, cur_height);
        spr_render(cur_width, cur_height);
        sg_end_pass();
        sg_commit();

        SDL_GL_SwapWindow(window);
    }

    spr_shutdown();
    sg_shutdown();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}

void read_file_to_buffer(const char* filename, char** buffer, size_t* len)
{
    TX_ASSERT(buffer);

    size_t string_size = 0, read_size = 0;
    FILE* file = fopen(filename, "r");

    if (file) {
        fseek(file, 0, SEEK_END);
        long tell = ftell(file);

        if (tell < 0) {
            fclose(file);
            return;
        }

        string_size = (size_t)tell;

        rewind(file);
        *buffer = (char*)calloc(string_size + 1, sizeof(char));
        read_size = fread(*buffer, sizeof(char), string_size, file);
        (*buffer)[read_size + 1] = '\0';
        fclose(file);
    }

    if (len) {
        *len = read_size;
    }
}
