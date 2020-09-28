#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord0;
layout(location = 2) in vec3 inst_pos;
layout(location = 3) in vec4 spr_rect;   // pos: x,y -- size: z,w
layout(location = 4) in vec2 spr_origin; // 0,0 : top left, 0.5,0.5: center, 1,1: bottom right

uniform mat4 view_proj;
out vec2 uv;

void main()
{
    uv = texcoord0 * spr_rect.zw + spr_rect.xy;
    gl_Position =
        view_proj
        * vec4(position + inst_pos - vec3(clamp(spr_origin, vec2(0, 0), vec2(1, 1)), 0.0f), 1.0f);
}
