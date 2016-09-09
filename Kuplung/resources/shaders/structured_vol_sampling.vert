#version 410 core

in vec3 a_vertexPosition;

void main() {
//    gl_Position = vec4(a_vertexPosition.xy, 0.0, 1.0);
    gl_Position = vec4(a_vertexPosition, 1.0);
}
