#version 410 core

uniform mat4 u_MVPMatrix;

in vec3 a_vertexPosition;
in vec3 a_vertexNormal;

out vec3 v_vertexNormal;

void main(void) {
    v_vertexNormal = a_vertexNormal;
    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
