#version 410 core

uniform mat4 u_MVPMatrix;

in vec3 a_vertexPosition;
in vec3 a_vertexNormal;
in vec4 a_color;

out vec3 v_vertexNormal;
out vec4 v_color;

void main(void) {
    v_vertexNormal = a_vertexNormal;
    v_color = a_color;

    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
