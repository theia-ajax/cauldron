#include "sprite_draw.h"

#include "futils.h"
#include "stb_image.h"
#include "string.h"

struct vertex {
    vec3 pos;
    vec2 uv;
};

struct sprite {
    vec3 pos;
    vec4 rect;
    vec2 origin;
};

typedef struct uniform_block {
    mat4 view_proj;
    float sprite_size;
} uniform_block;

#define K_MAX_SPRITES 16384

struct sprite sprites[K_MAX_SPRITES];

sg_buffer geom_vbuf;
sg_buffer geom_ibuf;

sg_buffer inst_vbuf;

sg_image atlas;

uint32_t sprite_ct;
float pixels_per_meter = 8.0f;

const uint32_t k_canvas_width = 256 * 2;
const uint32_t k_canvas_height = 144 * 2;

struct {
    sg_shader shader;
    sg_pipeline pip;
    sg_bindings bindings;
    sg_pass pass;
    sg_pass_action pass_action;
    sg_image color_img;
    sg_image depth_img;
} canvas;

struct {
    sg_shader shader;
    sg_pipeline pip;
    sg_bindings bindings;
    sg_pass_action pass_action;
} screen;

void spr_init()
{
    // Configure render target render
    int iw, ih, ichan;
    stbi_uc* pixels = stbi_load("assets/atlas2.png", &iw, &ih, &ichan, 4);
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

    const float k_size = 1.0f;
    struct vertex quad_verts[] = {
        {.pos = {.x = 0, .y = 0}, .uv = {.x = 0.0f, .y = 0.0f}},
        {.pos = {.x = k_size, .y = 0}, .uv = {.x = 1.0f, .y = 0.0f}},
        {.pos = {.x = k_size, .y = k_size}, .uv = {.x = 1.0f, .y = 1.0f}},
        {.pos = {.x = 0, .y = k_size}, .uv = {.x = 0.0f, .y = 1.0f}},
    };

    const uint16_t quad_indices[] = {0, 2, 3, 0, 1, 2};

    geom_vbuf = sg_make_buffer(&(sg_buffer_desc){
        .content = quad_verts,
        .size = sizeof(quad_verts),
    });

    geom_ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .content = quad_indices,
        .size = sizeof(quad_indices),
    });

    inst_vbuf = sg_make_buffer(&(sg_buffer_desc){
        .usage = SG_USAGE_STREAM,
        .size = sizeof(sprites),
    });

    {
        char* vs_buffer;
        char* fs_buffer;
        size_t vs_len, fs_len;

        enum tx_result vs_result =
            read_file_to_buffer("assets/shaders/sprite.vert", &vs_buffer, &vs_len);
        enum tx_result fs_result =
            read_file_to_buffer("assets/shaders/sprite.frag", &fs_buffer, &fs_len);

        TX_ASSERT(vs_result == TX_SUCCESS && fs_result == TX_SUCCESS);

        canvas.shader = sg_make_shader(&(sg_shader_desc){
            .vs.uniform_blocks[0] =
                {
                    .size = sizeof(uniform_block),
                    .uniforms =
                        {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "sprite_size", .type = SG_UNIFORMTYPE_FLOAT},
                        },
                },
            .fs.images[0] = {.name = "atlas", .type = SG_IMAGETYPE_2D},
            .vs.source = vs_buffer,
            .fs.source = fs_buffer,
        });

        free(vs_buffer);
        free(fs_buffer);
    }

    sg_image_desc image_desc = (sg_image_desc){
        .render_target = true,
        .width = k_canvas_width,
        .height = k_canvas_height,
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
    };

    canvas.color_img = sg_make_image(&image_desc);
    image_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    canvas.depth_img = sg_make_image(&image_desc);

    canvas.pass = sg_make_pass(&(sg_pass_desc){
        .color_attachments[0].image = canvas.color_img,
        .depth_stencil_attachment.image = canvas.depth_img,
    });

    // default render states are fine for triangle
    canvas.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = canvas.shader,
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
                                .format = SG_VERTEXFORMAT_FLOAT4,
                                .offset = 12,
                                .buffer_index = 1,
                            },
                        [4] =
                            {
                                .format = SG_VERTEXFORMAT_FLOAT2,
                                .offset = 28,
                                .buffer_index = 1,
                            },
                    },
            },
        .depth_stencil =
            {
                .depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
                .depth_write_enabled = true,
            },
        .blend =
            {
                .enabled = true,
                .depth_format = SG_PIXELFORMAT_DEPTH,
                .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .src_factor_alpha = SG_BLENDFACTOR_ONE,
                .dst_factor_alpha = SG_BLENDFACTOR_ZERO,
            },
        .rasterizer.cull_mode = SG_CULLMODE_BACK,
    });

    canvas.bindings = (sg_bindings){
        .vertex_buffers =
            {
                [0] = geom_vbuf,
                [1] = inst_vbuf,
            },
        .index_buffer = geom_ibuf,
        .fs_images[0] = atlas,
    };

    // Configure screen full-screen quad render
    {
        char* vs_buffer;
        char* fs_buffer;
        size_t vs_len, fs_len;

        enum tx_result vs_result =
            read_file_to_buffer("assets/shaders/fullscreen_quad.vert", &vs_buffer, &vs_len);
        enum tx_result fs_result =
            read_file_to_buffer("assets/shaders/fullscreen_quad.frag", &fs_buffer, &fs_len);

        TX_ASSERT(vs_result == TX_SUCCESS && fs_result == TX_SUCCESS);

        screen.shader = sg_make_shader(&(sg_shader_desc){
            .fs.images[0] = {.name = "screen_texture", .type = SG_IMAGETYPE_2D},
            .vs.source = vs_buffer,
            .fs.source = fs_buffer,
        });

        free(vs_buffer);
        free(fs_buffer);
    }

    // Our fullscreen quad shader doesn't require any attributes but sokol has no mechanism for
    // for binding empty attribute arrays so define a per-instance float so *something* goes across
    // and sokol is happy.
    screen.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = screen.shader,
        .layout =
            {
                .buffers[0] =
                    {
                        .step_func = SG_VERTEXSTEP_PER_INSTANCE,
                    },
                .attrs[0] =
                    {
                        .format = SG_VERTEXFORMAT_FLOAT,
                    },
            },
    });

    float data = 0.0f;
    screen.bindings = (sg_bindings){
        .vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
            .usage = SG_USAGE_IMMUTABLE,
            .size = sizeof(float),
            .content = &data,
        }),
        .fs_images[0] = canvas.color_img,
    };

    screen.pass_action = (sg_pass_action){
        .colors[0] = {.action = SG_ACTION_CLEAR, .val = {0.1f, 0.0f, 0.1f}},
    };
}

void spr_shutdown()
{
}

void spr_render(int width, int height)
{
    sg_update_buffer(inst_vbuf, sprites, sizeof(struct sprite) * sprite_ct);

    {
        const float aspect = (float)k_canvas_width / k_canvas_height;
        float view_width = k_canvas_width / pixels_per_meter;
        float view_height = k_canvas_height / pixels_per_meter;

        mat4 view = mat4_look_at((vec3){1, 1, 100}, (vec3){1, 1, -1}, (vec3){0, 1, 0});
        mat4 projection = mat4_ortho(0, view_width, view_height, 0, 0.0f, 250.0f);
        mat4 view_proj = mat4_mul(projection, view);

        sg_begin_pass(
            canvas.pass,
            &(sg_pass_action){
                .colors[0] =
                    {
                        .action = SG_ACTION_CLEAR,
                        .val = {0.0f, 0.0f, 0.0f, 1.0f},
                    },
            });

        sg_apply_pipeline(canvas.pip);
        sg_apply_bindings(&canvas.bindings);
        uniform_block uniforms = {
            .view_proj = view_proj,
            .sprite_size = 2.0f,
        };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &uniforms, sizeof(uniform_block));
        sg_draw(0, 6, sprite_ct);
        sg_end_pass();

        sg_begin_default_pass(&screen.pass_action, width, height);
        sg_apply_pipeline(screen.pip);
        sg_apply_bindings(&screen.bindings);
        sg_draw(0, 6, 1);
        sg_end_pass();
    }

    sprite_ct = 0;
}

void spr_draw(const sprite_draw_desc* desc)
{
    TX_ASSERT(desc);
    TX_ASSERT(sprite_ct < K_MAX_SPRITES);

    uint32_t row = desc->sprite_id / 16;
    uint32_t col = desc->sprite_id % 16;

    const float flip_offsets[3] = {0.0f, 1.0f, 1.0f};
    const float flip_scales[3] = {1.0f, -1.0f, -1.0f};

    float fx = flip_offsets[desc->flip & SPRITE_FLIP_X];
    float fy = flip_offsets[desc->flip & SPRITE_FLIP_Y];

    float fw = flip_scales[desc->flip & SPRITE_FLIP_X];
    float fh = flip_scales[desc->flip & SPRITE_FLIP_Y];

    sprites[sprite_ct] = (struct sprite){
        .pos =
            (vec3){
                .x = desc->pos.x,
                .y = desc->pos.y,
                .z = desc->layer,
            },
        .rect = {(col + fx) / 16.0f, (row + fy) / 16.0f, fw / 16, fh / 16},
        .origin = desc->origin,
    };
    sprite_ct++;
}