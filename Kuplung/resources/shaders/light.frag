#version 410

uniform sampler2D u_sampler;
uniform bool fs_useColor;
uniform vec3 fs_color;
in vec2 v_textureCoord;
out vec4 fragColor;

void main(void) {
    if (fs_useColor)
        fragColor = vec4(fs_color, 1.0);
    else
        fragColor = texture(u_sampler, v_textureCoord);
}
