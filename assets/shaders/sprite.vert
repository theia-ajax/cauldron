#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord0;
layout(location = 2) in vec3 inst_pos;
layout(location = 3) in vec4 spr_rect; // pos: x,y -- size: z,w

uniform mat4 view_proj;
out vec2 uv;

void main()
{
    uv = texcoord0 * spr_rect.zw + spr_rect.xy;
    gl_Position = view_proj * vec4(position + inst_pos, 1.0f);
}
