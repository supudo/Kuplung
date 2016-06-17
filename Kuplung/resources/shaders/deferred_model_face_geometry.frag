#version 410 core

in vec2 fs_texCoord0;
in vec3 fs_normal0;
in vec3 fs_worldPos0;

layout (location = 0) out vec3 fs_out_WorldPos;
layout (location = 1) out vec3 fs_out_Diffuse;
layout (location = 2) out vec3 fs_out_Normal;
layout (location = 3) out vec3 fs_out_TexCoord;

uniform sampler2D fs_samplerDiffuse;

void main() {
    fs_out_WorldPos = fs_worldPos0;
    fs_out_Diffuse = texture(fs_samplerDiffuse, fs_texCoord0).xyz;
    fs_out_Normal = normalize(fs_normal0);
    fs_out_TexCoord = vec3(fs_texCoord0, 0.0);
}
