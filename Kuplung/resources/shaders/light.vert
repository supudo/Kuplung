#version 410

uniform mat4 u_MVPMatrix;

in vec3 a_vertexPosition;
in vec3 a_vertexNormal;
in vec2 a_textureCoord;

out vec3 v_vertexNormal;
out vec2 v_textureCoord;

void main(void) {
    v_vertexNormal = a_vertexNormal;
    v_textureCoord = a_textureCoord;

    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
