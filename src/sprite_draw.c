#include "sprite_draw.h"

#include "string.h"

struct vertex {
    vec3 pos;
    vec4 color;
    vec2 uv;
};

const struct vertex k_quad_verts[] = {
    {.pos = {.x = -0.5f, .y = 0.5f}},
    {.pos = {.x = 0.5f, .y = 0.5f}},
    {.pos = {.x = 0.5f, .y = -0.5f}},
    {.pos = {.x = -0.5f, .y = -0.5f}},
};

const uint16_t k_quad_indices[] = {0, 2, 3, 0, 1, 2};

typedef struct uniform_block {
    mat4 view_proj;
} uniform_block;

sg_buffer vert_buf;
sg_buffer idx_buf;

sg_shader spr_shader;
sg_pipeline spr_pip;
sg_bindings spr_binds;

uint32_t sprite_ct;

void spr_init()
{
    vert_buf = sg_make_buffer(&(sg_buffer_desc){
        .usage = SG_USAGE_DYNAMIC,
        .content = NULL,
        .size = sizeof(struct vertex) * 1024,
    });

    idx_buf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .usage = SG_USAGE_DYNAMIC,
        .content = NULL,
        .size = sizeof(uint16_t) * 6 * 1024,
    });

    spr_shader = sg_make_shader(&(sg_shader_desc){
        .vs.uniform_blocks[0] =
            {
                .size = sizeof(uniform_block),
                .uniforms =
                    {
                        [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    },
            },
        //.fs.images[0] = {.name = "tex", .type = SG_IMAGETYPE_2D},
        .vs.source = "#version 330\n"
                     "layout(location=0) in vec3 position;\n"
                     "layout(location=1) in vec4 color;\n"
                     "layout(location=2) in vec2 texcoord0;\n"
                     "uniform mat4 view_proj;\n"
                     "void main() {\n"
                     "  diffuse = vec3(1, 1, 1) * (1 - dot(light_dir, world_norm));\n"
                     "  uv = texcoord0;\n"
                     "  gl_Position = view_proj * vec4(position, 1.0f);\n"
                     "}\n",
        .fs.source = "#version 330\n"
                     //"uniform sampler2D tex;\n"
                     "in vec3 baseColor;\n"
                     "in vec2 uv;\n"
                     "out vec4 frag_color;\n"
                     "void main() {\n"
                     "  frag_color = vec4(1, 0, 0, 1);\n"
                     //"  frag_color = baseColor * texture(tex, uv);\n"
                     "}\n"});

    // default render states are fine for triangle
    spr_pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = spr_shader,
        .index_type = SG_INDEXTYPE_UINT16,
        .layout =
            {
                .attrs =
                    {
                        [0].format = SG_VERTEXFORMAT_FLOAT3,
                        [1].format = SG_VERTEXFORMAT_FLOAT4,
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

    spr_binds = (sg_bindings){
        .vertex_buffers[0] = vert_buf,
        .index_buffer = idx_buf,
    };
    // .fs_images[0] = texture,
}

void spr_shutdown() {}

void spr_render(int width, int height)
{
    mat4 view = mat4_look_at((vec3){0, 0, -1.0f}, (vec3){0, 0, 1}, (vec3){0, 1, 0});
    mat4 projection = mat4_ortho(-1.0f, 1.0f, -1.f, 1.f, 0.f, 10.0f);
    mat4 view_proj = mat4_mul(projection, view);

    {
        sg_apply_pipeline(spr_pip);
        sg_apply_bindings(&spr_binds);
        uniform_block uniforms = {
            .view_proj = view_proj,
        };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &uniforms, sizeof(uniform_block));
        sg_draw(0, sprite_ct * 6, 1);
    }

    sprite_ct = 0;
    sg_update_buffer(vert_buf, NULL, 0);
    sg_update_buffer(idx_buf, NULL, 0);
}

void spr_draw(vec3 pos)
{
    struct vertex verts[4];
    uint16_t idx[6];

    for (int i = 0; i < 4; ++i) {
        verts[i] = k_quad_verts[i];
        verts[i].pos = vec3_add(verts[i].pos, pos);
    }

    for (int i = 0; i < 6; ++i) {
        idx[i] = k_quad_indices[i] + sprite_ct * 4;
    }

    sg_append_buffer(vert_buf, verts, sizeof(verts));
    sg_append_buffer(idx_buf, idx, sizeof(idx));

    sprite_ct++;
}