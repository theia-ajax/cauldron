
#include "tx_types.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl3w.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_ASSERT TX_ASSERT
#include "sokol_gfx.h"

#define TX_MATH_IMPL
#include "tx_math.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

typedef struct vertex {
    vec3 pos;
    vec3 norm;
    vec2 uv;
} vertex;

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

    cgltf_options options = {0};
    cgltf_data* data = NULL;
    if (cgltf_parse_file(&(cgltf_options){0}, "assets/carPolice.gltf", &data)) {

        cgltf_free(data);
    }

    const vertex vertices[] = {
        {.pos = {.x = -0.5f, .y = 0.5f, .z = -0.5f}, .uv = {.x = 0.0f, .y = 0.0f}},
        {.pos = {.x = 0.5f, .y = 0.5f, .z = -0.5f}, .uv = {.x = 1.0f, .y = 0.0f}},
        {.pos = {.x = 0.5f, .y = -0.5f, .z = -0.5f}, .uv = {.x = 1.0f, .y = 1.0f}},
        {.pos = {.x = -0.5f, .y = -0.5f, .z = -0.5f}, .uv = {.x = 0.0f, .y = 1.0f}},
        {.pos = {.x = -0.5f, .y = 0.5f, .z = 0.5f}, .uv = {.x = 0.0f, .y = 0.0f}},
        {.pos = {.x = 0.5f, .y = 0.5f, .z = 0.5f}, .uv = {.x = 1.0f, .y = 0.0f}},
        {.pos = {.x = 0.5f, .y = -0.5f, .z = 0.5f}, .uv = {.x = 1.0f, .y = 1.0f}},
        {.pos = {.x = -0.5f, .y = -0.5f, .z = 0.5f}, .uv = {.x = 0.0f, .y = 1.0f}},
    };

    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .content = vertices,
    });

    // clang-format off
    const uint16_t indices[] = {
        0, 2, 1, 0, 3, 2,   // front
        4, 5, 6, 4, 6, 7,   // back
        7, 3, 0, 0, 4, 7,
        5, 2, 6, 5, 1, 2,
    };
    // clang-format on

    sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(indices),
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .content = indices,
    });

    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        .vs.source = "#version 330\n"
                     "layout(location=0) in vec4 position;\n"
                     "layout(location=1) in vec3 norm;\n"
                     "layout(location=2) in vec2 uv;\n"
                     "uniform mat4 MVP;\n"
                     "uniform vec3 light_dir;"
                     "out vec4 color;\n"
                     "void main() {\n"
                     "  gl_Position = MVP * position;\n"
                     "  color = vec4(1, 0, 0, 1) * (1.0f - dot(light_dir, norm));\n"
                     "}\n",
        .vs.uniform_blocks[0] =
            {
                .size = sizeof(mat4),
                .uniforms =
                    {
                        {
                            .name = "MVP",
                            .type = SG_UNIFORMTYPE_MAT4,
                        },
                    },
            },
        .fs.source = "#version 330\n"
                     "in vec4 color;\n"
                     "out vec4 frag_color;\n"
                     "void main() {\n"
                     "  frag_color = color;\n"
                     "}\n"});

    // default render states are fine for triangle
    sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .layout =
            {
                .attrs =
                    {
                        [0].format = SG_VERTEXFORMAT_FLOAT3,
                        [1].format = SG_VERTEXFORMAT_FLOAT3,
                        [2].format = SG_VERTEXFORMAT_FLOAT2,
                    },
            },
        .depth_stencil =
            {
                .depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
                .depth_write_enabled = true,
            },
        .rasterizer.cull_mode = SG_CULLMODE_BACK,
    });

    sg_bindings binds = {
        .vertex_buffers[0] = vbuf,
        .index_buffer = ibuf,
    };

    // default: clear to grey
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

        static float r = 0.0f;
        mat4 view = mat4_look_at((vec3){0, 0, 3}, (vec3){0, 0, 0}, (vec3){0, 1, 0});
        mat4 projection = mat4_perspective(K_TX_PI / 4.0f, 16.0f / 9, 0.1f, 100.0f);
        r += 0.01f;
        mat4 model = mat4_rotate_y(mat4_identity(), r);

        mat4 mvp = mat4_mul(mat4_mul(projection, view), model);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, mvp.d, sizeof(mat4));

        sg_apply_bindings(&binds);
        sg_draw(0, sizeof(indices) / sizeof(uint16_t), 1);
        sg_end_pass();
        sg_commit();

        SDL_GL_SwapWindow(window);
    }

    sg_shutdown();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}