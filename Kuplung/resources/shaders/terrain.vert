#version 410 core

uniform mat4 u_MVPMatrix;

in vec3 a_vertexPosition;
in vec4 a_color;

out vec4 v_color;

void main(void) {
    v_color = a_color;

    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
