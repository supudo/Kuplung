#version 410 core

in vec3 v_color;
out vec4 fragColor;

void main(void) {
  if (fract(out_UV.x / 0.001f) < 0.01f || fract(out_UV.y / 0.001f) < 0.01f)
    fragColor = v_color;
  else
    fragColor = vec4(1, 1, 1, 0);
}
