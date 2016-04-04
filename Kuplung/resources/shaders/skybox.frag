#version 410 core

uniform samplerCube u_sampler;
in vec3 vs_textureCoord;
out vec4 fragColor;

void main(void) {
    fragColor = texture(u_sampler, vs_textureCoord);
}
