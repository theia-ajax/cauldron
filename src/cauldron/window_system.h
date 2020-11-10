#pragma once

#include "game_systems_forward.h"
#include "tx_types.h"

typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

tx_result window_system_init(game_settings* settings);
void window_system_term(void);
void window_swap(void);
void window_get_size(int* x, int* y);
SDL_Window* window_get_ptr(void);
SDL_GLContext window_get_gl(void);

tx_result imgui_system_init(game_settings* settings);
void imgui_system_term(void);
void imgui_begin(float dt);
void imgui_end(void);