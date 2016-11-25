#version 410 core

in vec3 position;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}
