#version 410 core

uniform mat4 u_MVPMatrix;

in vec3 a_vertexPosition;
in vec3 fs_color;

out vec3 v_color;

void main(void) {
    v_color = fs_color;
    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
