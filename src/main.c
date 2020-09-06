
#include "tx_types.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl3w.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_ASSERT TX_ASSERT
#include "sokol_gfx.h"

int main(int argc, char* argv[])
{
    SDL_Window* window = SDL_CreateWindow(
        "cauldron",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    TX_ASSERT(gl3wInit() == GL3W_OK);

    /* setup sokol_gfx */
    sg_setup(&(sg_desc){0});

    /* a vertex buffer */
    const float vertices[] = {// positions            // colors
                              0.0f, 0.5f, 0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f,
                              1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  1.0f};
    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .content = vertices,
    });

    /* a shader */
    sg_shader shd =
        sg_make_shader(&(sg_shader_desc){.vs.source = "#version 330\n"
                                                      "layout(location=0) in vec4 position;\n"
                                                      "layout(location=1) in vec4 color0;\n"
                                                      "out vec4 color;\n"
                                                      "void main() {\n"
                                                      "  gl_Position = position;\n"
                                                      "  color = color0;\n"
                                                      "}\n",
                                         .fs.source = "#version 330\n"
                                                      "in vec4 color;\n"
                                                      "out vec4 frag_color;\n"
                                                      "void main() {\n"
                                                      "  frag_color = color;\n"
                                                      "}\n"});

    /* a pipeline state object (default render states are fine for triangle) */
    sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {[0].format = SG_VERTEXFORMAT_FLOAT3, [1].format = SG_VERTEXFORMAT_FLOAT4}}});

    /* resource bindings */
    sg_bindings binds = {.vertex_buffers[0] = vbuf};

    /* default pass action (clear to grey) */
    sg_pass_action pass_action = {0};

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
                break;
            }
        }

        int cur_width, cur_height;
        SDL_GetWindowSize(window, &cur_width, &cur_height);
        sg_begin_default_pass(&pass_action, cur_width, cur_height);
        sg_apply_pipeline(pip);
        sg_apply_bindings(&binds);
        sg_draw(0, 3, 1);
        sg_end_pass();
        sg_commit();

        SDL_GL_SwapWindow(window);
    }

    sg_shutdown();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}