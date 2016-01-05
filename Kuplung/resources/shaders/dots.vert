uniform mat4 u_MVPMatrix;

in vec3 a_vertexPosition;

void main(void) {
    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
