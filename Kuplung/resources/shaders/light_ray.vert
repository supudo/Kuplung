#version 410 core

layout (location = 0) in vec3 a_vertexPosition;

uniform mat4 u_MVPMatrix;

void main(void) {
    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
