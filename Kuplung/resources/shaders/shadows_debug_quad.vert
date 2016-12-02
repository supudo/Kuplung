#version 410 core

layout (location = 0) in vec3 vs_vertexPosition;
layout (location = 1) in vec2 vs_textureCoord;

out vec2 TexCoords;

void main() {
    gl_Position = vec4(vs_vertexPosition, 1.0f);
    TexCoords = vs_textureCoord;
}
