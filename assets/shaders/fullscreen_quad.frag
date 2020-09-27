#version 330

uniform sampler2D screen_texture;
in vec2 uv;

out vec4 frag_color;

void main()
{
    frag_color = texture(screen_texture, uv);
}
