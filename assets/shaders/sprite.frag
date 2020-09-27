#version 330

uniform sampler2D atlas;

in vec2 uv;

out vec4 frag_color;

void main()
{
    frag_color = texture(atlas, uv);
    if (frag_color.rgb == vec3(0, 0, 0)) {
        frag_color.a = 0.0;
    }
}
