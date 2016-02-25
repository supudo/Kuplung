#version 410

uniform bool a_actAsMirror;
uniform float a_alpha;

out vec4 fragColor;

void main(void) {
    if (a_actAsMirror)
        fragColor = vec4(1.0, 1.0, 1.0, a_alpha);
    else
        fragColor = vec4(0.7, 0.7, 0.7, a_alpha);
}
