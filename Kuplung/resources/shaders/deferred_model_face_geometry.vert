#version 410 core

layout (location = 0) in vec3 vs_vertexPosition;
layout (location = 1) in vec2 vs_textureCoord;
layout (location = 2) in vec3 vs_vertexNormal;

uniform mat4 vs_MVPMatrix;
uniform mat4 vs_ModelMatrix;

out vec3 fs_worldPos0;
out vec2 fs_texCoord0;
out vec3 fs_normal0;

void main() {
    gl_Position = vs_MVPMatrix * vec4(vs_vertexPosition, 1.0);

    fs_worldPos0 = (vs_ModelMatrix * vec4(vs_vertexPosition, 1.0)).xyz;
    fs_texCoord0 = vs_textureCoord;
    fs_normal0 = (vs_ModelMatrix * vec4(vs_vertexNormal, 0.0)).xyz;
}
