#version 410 core

in vec3 fs_color;
out vec4 fragColor;

void main(void) {
  fragColor = vec4(fs_color, 1.0);
}
