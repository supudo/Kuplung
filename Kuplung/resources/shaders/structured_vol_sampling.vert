#version 410 core

in vec3 a_vertexPosition;
uniform mat4 vs_MVPMatrix;

void main() {
    gl_Position = vs_MVPMatrix * vec4(a_vertexPosition.xy, 0.0, 1.0);
}
