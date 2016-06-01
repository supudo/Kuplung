#version 410 core

in vec2 v_textureCoord;
in vec3 v_vertexNormal;
in vec4 v_color;

uniform bool has_texture;
uniform sampler2D sampler_texture;

out vec4 fragColor;

void main(void) {
    if (has_texture)
        fragColor = texture(sampler_texture, v_textureCoord);
    else
        fragColor = v_color;
}
