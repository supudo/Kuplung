#version 410

uniform vec3 a_vertexNormal;
uniform bool a_actAsMirror;

out vec4 fragColor;

void main(void) {
    if (a_actAsMirror) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
        fragColor = vec4(0.7, 0.7, 0.7, 1.0);
}
