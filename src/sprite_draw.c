#include "sprite_draw.h"

#include "stb_image.h"
#include "string.h"

struct vertex {
    vec3 pos;
    vec2 uv;
};

struct sprite {
    vec3 pos;
    float spr_id;
};

const struct vertex k_quad_verts[] = {
    {.pos = {.x = -0.5f, .y = 0.5f}, .uv = {.x = 0.0f, .y = 0.0f}},
    {.pos = {.x = 0.5f, .y = 0.5f}, .uv = {.x = 1.0f / 16, .y = 0.0f}},
    {.pos = {.x = 0.5f, .y = -0.5f}, .uv = {.x = 1.0f / 16, .y = 1.0f / 16}},
    {.pos = {.x = -0.5f, .y = -0.5f}, .uv = {.x = 0.0f, .y = 1.0f / 16}},
};

const uint16_t k_quad_indices[] = {0, 2, 3, 0, 1, 2};

typedef struct uniform_block {
    mat4 view_proj;
} uniform_block;

#define K_MAX_SPRITES 16384

struct sprite sprites[K_MAX_SPRITES];

sg_buffer geom_vbuf;
sg_buffer geom_ibuf;

sg_buffer inst_vbuf;

sg_shader spr_shader;
sg_pipeline spr_pip;
sg_bindings spr_binds;

sg_image atlas;

uint32_t sprite_ct;
float view_size = 8.0f;

void spr_init()
{
    int iw, ih, ichan;
    stbi_uc* pixels = stbi_load("assets/atlas.png", &iw, &ih, &ichan, 4);
    TX_ASSERT(pixels);

    atlas = sg_make_image(&(sg_image_desc){
        .width = iw,
        .height = ih,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .content.subimage[0][0] =
            {
                .ptr = pixels,
                .size = iw * ih * ichan,
            },
    });

    geom_vbuf = sg_make_buffer(&(sg_buffer_desc){
        .content = k_quad_verts,
        .size = sizeof(k_quad_verts),
    });

    geom_ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .content = k_quad_indices,
        .size = sizeof(k_quad_indices),
    });

    inst_vbuf = sg_make_buffer(&(sg_buffer_desc){
        .usage = SG_USAGE_STREAM,
        .size = sizeof(sprites),
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
        .fs.images[0] = {.name = "atlas", .type = SG_IMAGETYPE_2D},
        .vs.source = "#version 330\n"
                     "layout(location=0) in vec3 position;\n"
                     "layout(location=1) in vec2 texcoord0;\n"
                     "layout(location=2) in vec3 inst_pos;\n"
                     "layout(location=3) in float spr_id;\n"
                     "uniform mat4 view_proj;\n"
                     "out vec2 uv;\n"
                     "void main() {\n"
                     "  uv = texcoord0 + vec2(mod(spr_id, 16) / 16, (spr_id / 16) / 16);\n"
                     "  gl_Position = view_proj * vec4(position + inst_pos, 1.0f);\n"
                     "}\n",
        .fs.source = "#version 330\n"
                     "uniform sampler2D atlas;\n"
                     "in vec2 uv;\n"
                     "out vec4 frag_color;\n"
                     "void main() {\n"
                     "  frag_color = texture(atlas, uv);\n"
                     "  if (frag_color.rgb == vec3(0, 0, 0)) { frag_color.a = 0.0; }\n"
                     "}\n"});

    // default render states are fine for triangle
    spr_pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = spr_shader,
        .index_type = SG_INDEXTYPE_UINT16,
        .layout =
            {
                .buffers =
                    {
                        [0] = {.stride = sizeof(struct vertex)},
                        [1] =
                            {
                                .stride = sizeof(struct sprite),
                                .step_func = SG_VERTEXSTEP_PER_INSTANCE,
                            },
                    },
                .attrs =
                    {
                        [0] =
                            {
                                .format = SG_VERTEXFORMAT_FLOAT3,
                                .offset = 0,
                                .buffer_index = 0,
                            },
                        [1] =
                            {
                                .format = SG_VERTEXFORMAT_FLOAT2,
                                .offset = 12,
                                .buffer_index = 0,
                            },
                        [2] =
                            {
                                .format = SG_VERTEXFORMAT_FLOAT3,
                                .offset = 0,
                                .buffer_index = 1,
                            },
                        [3] =
                            {
                                .format = SG_VERTEXFORMAT_FLOAT,
                                .offset = 12,
                                .buffer_index = 1,
                            },
                    },
            },
        .index_type = SG_INDEXTYPE_UINT16,
        .depth_stencil =
            {
                .depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
                .depth_write_enabled = true,
            },
        .blend =
            {
                .enabled = true,
                .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .src_factor_alpha = SG_BLENDFACTOR_ONE,
                .dst_factor_alpha = SG_BLENDFACTOR_ZERO,
            },
        .rasterizer.cull_mode = SG_CULLMODE_BACK,
    });

    spr_binds = (sg_bindings){
        .vertex_buffers =
            {
                [0] = geom_vbuf,
                [1] = inst_vbuf,
            },
        .index_buffer = geom_ibuf,
        .fs_images[0] = atlas,
    };
}

void spr_shutdown() {}

void spr_render(int width, int height)
{
    sg_update_buffer(inst_vbuf, sprites, sizeof(struct sprite) * sprite_ct);

    {
        float aspect = (float)width / height;
        float half_w = view_size * aspect;
        float half_h = view_size;

        mat4 view = mat4_look_at((vec3){0, 0, 1.0f}, (vec3){0, 0, -1}, (vec3){0, 1, 0});
        mat4 projection = mat4_ortho(-half_w, half_w, -half_h, half_h, 0.f, 10.0f);
        mat4 view_proj = mat4_mul(projection, view);
        sg_apply_pipeline(spr_pip);
        sg_apply_bindings(&spr_binds);
        uniform_block uniforms = {
            .view_proj = view_proj,
        };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &uniforms, sizeof(uniform_block));
        sg_draw(0, 6, sprite_ct);
    }

    sprite_ct = 0;
}

void spr_draw(vec3 pos)
{
    TX_ASSERT(sprite_ct < K_MAX_SPRITES);
    sprites[sprite_ct] = (struct sprite){
        .pos = pos,
        .spr_id = 1,
    };
    sprite_ct++;
}