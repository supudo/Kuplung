#version 410 core

layout (location = 0) in vec3 a_vertexPosition;
layout (location = 1) in vec3 a_color;

uniform mat4 u_MVPMatrix;

out vec3 fs_color;

void main(void) {
  fs_color = a_color;
  gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
