#version 410 core

in vec3 vs_vertexPosition;
uniform mat4 shadow_lightSpaceMatrix;
uniform mat4 shadow_model;

void main() {
    gl_Position = shadow_lightSpaceMatrix * shadow_model * vec4(vs_vertexPosition, 1.0f);
}
