#version 410 core

layout (location = 0) in vec3 vs_vertexPosition;

uniform mat4 vs_MatrixView;
uniform mat4 vs_MatrixProjection;

out vec3 vs_textureCoord;

void main(void) {
    vec4 vpos = vs_MatrixProjection * vs_MatrixView * vec4(vs_vertexPosition, 1.0);
    gl_Position = vpos.xyww;
    vs_textureCoord = vs_vertexPosition;
}
