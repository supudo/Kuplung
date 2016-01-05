uniform mat4 u_MVPMatrix;

in vec3 a_vertexPosition;
in vec4 a_axisColor;

out vec4 v_axisColor;

void main(void) {
    v_axisColor = a_axisColor;

    gl_Position = u_MVPMatrix * vec4(a_vertexPosition, 1.0);
}
