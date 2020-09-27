
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

    vec3 snake_targ[128] = {0};
    vec3 snake_pos[128] = {0};
    vec3 snake_vel[128] = {0};

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
        vec2 input = {0};

        if (txinp_get_key(TXINP_KEY_UP)) {
            input.y += 1.0f;
        }

        if (txinp_get_key(TXINP_KEY_DOWN)) {
            input.y -= 1.0f;
        }

        if (txinp_get_key(TXINP_KEY_RIGHT)) {
            input.x += 1.0f;
        }

        if (txinp_get_key(TXINP_KEY_LEFT)) {
            input.x -= 1.0f;
        }

        if (vec2_len2(input) > 0) {
            input = vec2_norm(input);
        }

        for (int i = 127; i > 0; --i) {
            snake_targ[i] = snake_targ[i - 1];
        }
        snake_targ[0] = vec3_add(snake_targ[0], vec2_vec3(vec2_scale(input, 10.0f * dt)));
        snake_pos[0] = snake_targ[0];

        const float max_spd = 50.0f;
        for (int i = 1; i < 128; ++i) {
            vec3 delta = vec3_sub(snake_targ[i], snake_pos[i]);
            float dist = vec3_len(delta);
            vec3 ndelta = vec3_norm(delta);
            float accel = lerpf(0.0f, 100.0f, clampf(dist / 50.0f, 0.0f, 1.0f));
            if (vec3_dot(snake_vel[i], delta) < 0) {
                accel *= 2.0f;
            }
            snake_vel[i] =
                vec3_clamp_len(vec3_add(snake_vel[i], vec3_scale(ndelta, accel * dt)), max_spd);
            snake_pos[i] = vec3_add(snake_pos[i], vec3_scale(snake_vel[i], dt));
        }

        for (int i = 127; i >= 0; --i) {
            spr_draw(snake_pos[i]);
        }
        // spr_draw((vec3){0, 0, 0});

        // render
        int cur_width, cur_height;
        SDL_GetWindowSize(window, &cur_width, &cur_height);
        spr_render(cur_width, cur_height);
        sg_commit();

        SDL_GL_SwapWindow(window);
    }

    spr_shutdown();
    sg_shutdown();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}