
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

#define TX_RAND_IMPLEMENTATION
#include "tx_rand.h"

#define SNAKE_CHUNKS 128

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
    txrng_seed(0xFF0077FF);

    vec2 snake_targ[SNAKE_CHUNKS] = {0};
    vec2 snake_pos[SNAKE_CHUNKS] = {0};
    vec2 snake_vel[SNAKE_CHUNKS] = {0};

    for (int i = 0; i < SNAKE_CHUNKS; ++i)
        snake_pos[i] = (vec2){.x = 16, .y = 10};
    for (int i = 0; i < SNAKE_CHUNKS; ++i)
        snake_targ[i] = snake_pos[i];

    uint64_t last_ticks = SDL_GetPerformanceCounter();
    float time = 0.0f;
    bool is_running = true;
    while (is_running) {
        txinp_update();

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
            input.y -= 1.0f;
        }

        if (txinp_get_key(TXINP_KEY_DOWN)) {
            input.y += 1.0f;
        }

        if (txinp_get_key(TXINP_KEY_LEFT)) {
            input.x -= 1.0f;
        }

        if (txinp_get_key(TXINP_KEY_RIGHT)) {
            input.x += 1.0f;
        }

        for (int i = SNAKE_CHUNKS - 1; i > 0; --i) {
            snake_targ[i] = snake_targ[i - 1];
        }
        snake_targ[0] = vec2_add(snake_targ[0], vec2_scale(vec2_norm(input), 32.0f * dt));
        snake_pos[0] = snake_targ[0];

        const float max_spd = 50.0f;
        for (int i = 1; i < SNAKE_CHUNKS; ++i) {
            float r_ang = txrng_rangef(0.0f, K_TX_PI * 2.0f);
            float r_rad = txrng_rangef(0.0f, 10.0f);
            vec2 offset = (vec2){.x = cosf(r_ang) * r_rad, .y = sinf(r_ang) * r_rad};
            vec2 delta = vec2_sub(vec2_add(snake_targ[i], offset), snake_pos[i]);
            float dist = vec2_len(delta);
            vec2 ndelta = vec2_norm(delta);
            float accel = 70.0f - txrng_rangef(0.0f, 15.0f);
            if (vec2_dot(snake_vel[i], delta) < 0) {
                accel *= 2.0f;
            }
            snake_vel[i] = vec2_add(snake_vel[i], vec2_scale(ndelta, accel * dt));
            snake_vel[i] = vec2_sub(
                snake_vel[i],
                vec2_scale(vec2_norm(snake_vel[i]), fminf(2.0f, vec2_len(snake_vel[i]) * dt)));
            snake_pos[i] = vec2_add(snake_pos[i], vec2_scale(snake_vel[i], dt));

            for (int j = i + 1; j < SNAKE_CHUNKS; ++j) {
                vec2 d = vec2_sub(snake_pos[i], snake_pos[j]);
                if (vec2_len2(d) < 4) {
                    vec2 nd = vec2_norm(d);
                    snake_pos[i] = vec2_add(snake_pos[i], vec2_scale(nd, 0.1f * dt));
                    snake_pos[j] = vec2_add(snake_pos[j], vec2_scale(nd, -0.1f * dt));
                }
            }
        }

        for (int x = -32; x <= 32; x++) {
            for (int y = -18; y <= 18; y += 4) {
                spr_draw(&(sprite_draw_desc){
                    .sprite_id = 17,
                    .pos = (vec2){.x = (float)x, .y = (float)y},
                });
            }
        }

        for (int i = 127; i >= 0; --i) {
            spr_draw(&(sprite_draw_desc){
                .sprite_id = 1,
                .pos = snake_pos[i],
                .origin = (vec2){0.5f, 1.0f},
            });
        }

        // printf("pos: %02f, %02f\r", snake_pos[0].x, snake_pos[0].y);
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