#version 410 core

uniform mat4 vs_WorldMatrix;
in vec3 vs_vertexPosition;
out vec3 vs_textureCoord;

void main(void) {
    vec4 vpos = vs_WorldMatrix * vec4(vs_vertexPosition, 1.0);
    gl_Position = vpos.xyww;
    vs_textureCoord = vs_vertexPosition;
}
