#version 410 core

layout (location = 0) in vec4 a_vertexPosition;
layout (location = 1) in vec3 fs_color;

uniform mat4 u_MVPMatrix;
out vec3 v_color;

void main() {
  v_color = fs_color;
  gl_Position = u_MVPMatrix * a_vertexPosition;
}
