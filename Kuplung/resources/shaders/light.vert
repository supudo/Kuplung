#version 410 core

layout (location = 0) in vec3 a_vertexPosition;
layout (location = 1) in vec3 a_vertexNormal;
layout (location = 2) in vec2 a_textureCoord;

uniform mat4 u_MVPMatrix;

out vec3 v_vertexNormal;
out vec2 v_textureCoord;

void main(void) {
    v_vertexNormal = a_vertexNormal;
    v_textureCoord = a_textureCoord;

    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
