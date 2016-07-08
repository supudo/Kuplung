#version 410 core

layout (location = 0) in vec3 vs_vertexPosition;
layout (location = 1) in vec3 vs_vertexNormal;
layout (location = 2) in vec2 vs_textureCoord;
layout (location = 3) in vec3 vs_tangent;
layout (location = 4) in vec3 vs_bitangent;

uniform mat4 vs_MVPMatrix;

out vec3 fs_vertexPosition;
out vec3 fs_vertexNormal;
out vec2 fs_textureCoord;
out vec3 fs_tangent;
out vec3 fs_bitangent;

void main(void) {
    fs_vertexPosition = vs_vertexPosition;
    fs_vertexNormal = vs_vertexNormal;
    fs_textureCoord = vs_textureCoord;
    fs_tangent = vs_tangent;
    fs_bitangent = vs_bitangent;

    gl_Position = vs_MVPMatrix * vec4(vs_vertexPosition, 1.0);
}
