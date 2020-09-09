
#include "sokol_gfx.h"
#include "stb_ds.h"
#include "tinyobj_loader_c.h"
#include "tx_math.h"
#include "tx_types.h"
#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct vertex {
    vec3 pos;
    vec3 norm;
} vertex;

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

    // sg_buffer vbuf, ibuf;

    vertex* vertices = NULL;
    uint16_t* indices = NULL;

    {
        const char* filename = "assets/taxi2.obj";

        tinyobj_shape_t* shapes = NULL;
        tinyobj_material_t* materials = NULL;
        tinyobj_attrib_t attrib;

        size_t num_shapes;
        size_t num_materials;

        tinyobj_attrib_init(&attrib);

        int result = tinyobj_parse_obj(
            &attrib,
            &shapes,
            &num_shapes,
            &materials,
            &num_materials,
            filename,
            read_file_to_buffer,
            TINYOBJ_FLAG_TRIANGULATE);

        if (result == TINYOBJ_SUCCESS) {
            uint16_t curr_idx = 0;
            for (size_t shape_idx = 0; shape_idx < num_shapes; ++shape_idx) {
                const tinyobj_shape_t shape = shapes[shape_idx];
                for (uint32_t i = 0; i < shape.length; ++i) {
                    uint32_t face_idx = shape.face_offset + i;
                    uint32_t fnum = attrib.face_num_verts[face_idx];
                    TX_ASSERT(fnum == 3);

                    int32_t v_idx0 = attrib.faces[face_idx * fnum + 0].v_idx;
                    int32_t v_idx1 = attrib.faces[face_idx * fnum + 1].v_idx;
                    int32_t v_idx2 = attrib.faces[face_idx * fnum + 2].v_idx;

                    float* v_pos0 = &attrib.vertices[v_idx0 * 3];
                    float* v_pos1 = &attrib.vertices[v_idx1 * 3];
                    float* v_pos2 = &attrib.vertices[v_idx2 * 3];

                    int32_t vn_idx0 = attrib.faces[face_idx * fnum + 0].vn_idx;
                    int32_t vn_idx1 = attrib.faces[face_idx * fnum + 1].vn_idx;
                    int32_t vn_idx2 = attrib.faces[face_idx * fnum + 2].vn_idx;

                    float* v_norm0 = &attrib.normals[vn_idx0 * 3];
                    float* v_norm1 = &attrib.normals[vn_idx1 * 3];
                    float* v_norm2 = &attrib.normals[vn_idx2 * 3];

                    vertex v0 = (vertex){
                        .pos = {v_pos0[0], v_pos0[1], v_pos0[2]},
                        .norm = {v_norm0[0], v_norm0[1], v_norm0[2]},
                    };
                    vertex v1 = (vertex){
                        .pos = {v_pos1[0], v_pos1[1], v_pos1[2]},
                        .norm = {v_norm1[0], v_norm1[1], v_norm1[2]},
                    };
                    vertex v2 = (vertex){
                        .pos = {v_pos2[0], v_pos2[1], v_pos2[2]},
                        .norm = {v_norm2[0], v_norm2[1], v_norm2[2]},
                    };

                    arrput(vertices, v2);
                    arrput(vertices, v1);
                    arrput(vertices, v0);

                    arrput(indices, curr_idx++);
                    arrput(indices, curr_idx++);
                    arrput(indices, curr_idx++);
                    // arrput(vertices, (vertex){.pos = {v_pos1[0], v_pos1[1], v_pos1[2]}});
                    // arrput(vertices, (vertex){.pos = {v_pos2[0], v_pos2[1], v_pos2[2]}});

                    // attrib.faces[shape.face_offset + i].
                }
            }

            printf("#vertices: %d\n#faces: %d\n", (int)arrlen(vertices), (int)arrlen(indices) / 3);

            tinyobj_attrib_free(&attrib);
            if (shapes) {
                tinyobj_shapes_free(shapes, num_shapes);
            }
            if (materials) {
                tinyobj_materials_free(materials, num_materials);
            }
        }
    }

    // const vertex vertices[] = {
    //     {.pos = {.x = -0.5f, .y = 0.5f, .z = -0.5f}},
    //     {.pos = {.x = 0.5f, .y = 0.5f, .z = -0.5f}},
    //     {.pos = {.x = 0.5f, .y = -0.5f, .z = -0.5f}},
    //     {.pos = {.x = -0.5f, .y = -0.5f, .z = -0.5f}},
    //     {.pos = {.x = -0.5f, .y = 0.5f, .z = 0.5f}},
    //     {.pos = {.x = 0.5f, .y = 0.5f, .z = 0.5f}},
    //     {.pos = {.x = 0.5f, .y = -0.5f, .z = 0.5f}},
    //     {.pos = {.x = -0.5f, .y = -0.5f, .z = 0.5f}},
    // };

    // clang-format off
    // const uint16_t indices[] = {
    //     0, 2, 1, 0, 3, 2,   // front
    //     4, 5, 6, 4, 6, 7,   // back
    //     7, 3, 0, 0, 4, 7,
    //     5, 2, 6, 5, 1, 2,
    //     0, 1, 4, 5, 4, 1,
    //     2, 7, 6, 7, 2, 3,
    // };
    // clang-format on

    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .size = (int)arrlen(vertices) * (int)sizeof(vertex),
        .content = vertices,
    });

    sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
        .size = (int)arrlen(indices) * (int)sizeof(uint16_t),
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .content = indices,
    });

    sg_shader shd = sg_make_shader(
        &(sg_shader_desc){.vs.source = "#version 330\n"
                                       "layout(location=0) in vec3 position;\n"
                                       "layout(location=1) in vec3 norm;\n"
                                       "uniform mat4 MVP;\n"
                                       "out vec4 color;\n"
                                       "void main() {\n"
                                       "  gl_Position = MVP * vec4(position, 1.0f);\n"
                                       "  vec3 light_dir = normalize(vec3(0.7, -0.8, 0.1));"
                                       " vec3 diffuse = vec3(0.9764706, 0.7686275, 0.2745098);"
                                       "  color = vec4(diffuse * (1 - dot(light_dir, norm)), 1);\n"
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
        mat4 view = mat4_look_at((vec3){0, 0, 5}, (vec3){0, 0, 0}, (vec3){0, 1, 0});
        mat4 projection = mat4_perspective(K_TX_PI / 4.0f, 16.0f / 9, 0.1f, 100.0f);
        r += 0.01f;
        mat4 model =
            mat4_translate(mat4_rotate_y(mat4_identity(), r), 0.0f, sinf(r * 2.72f) * 0.5f, 0.0f);

        mat4 mvp = mat4_mul(mat4_mul(projection, view), model);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, mvp.d, sizeof(mat4));

        sg_apply_bindings(&binds);
        sg_draw(0, (int)arrlen(indices), 1);
        sg_end_pass();
        sg_commit();

        SDL_GL_SwapWindow(window);
    }

    sg_shutdown();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}