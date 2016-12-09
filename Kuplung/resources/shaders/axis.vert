#version 410 core

layout (location = 0) in vec3 a_vertexPosition;
layout (location = 1) in vec4 a_axisColor;

uniform mat4 u_MVPMatrix;

out vec4 v_axisColor;

void main(void) {
    v_axisColor = a_axisColor;

    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
