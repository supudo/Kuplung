#version 410 core

layout (location = 0) in vec3 a_vertexPosition;
layout (location = 1) in vec3 a_vertexNormal;

uniform mat4 u_MVPMatrix;

out vec3 v_vertexNormal;

void main(void) {
    v_vertexNormal = a_vertexNormal;
    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
