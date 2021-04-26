#include "system_sdl2.h"

#include "tx_input.h"
#include <SDL2/SDL.h>

void sdl2_term(ecs_world_t* world, void* ctx)
{
    SDL_Quit();
}

static void Sdl2ProcessEvents(ecs_iter_t* it)
{
    for (int i = 0; i < it->count; ++i) {
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
                    break;
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
    }
}

void SystemSdl2Import(ecs_world_t* world)
{
    ECS_MODULE(world, SystemSdl2);

    ecs_atfini(world, sdl2_term, NULL);

    ECS_TAG(world, Sdl2Input);

    ECS_SYSTEM(world, Sdl2ProcessEvents, EcsPostLoad, Sdl2Input);

    ECS_ENTITY(world, Sdl2, Sdl2Input);

    ECS_EXPORT_ENTITY(Sdl2);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        ecs_err("Unable to initialize SDL: %s", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    txinp_init();

    ecs_trace_1("SDL Initialized");
}