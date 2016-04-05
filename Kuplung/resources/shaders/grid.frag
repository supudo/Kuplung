#version 410 core

uniform bool a_actAsMirror;
uniform float a_alpha;
in vec3 v_color;

out vec4 fragColor;

void main(void) {
    if (a_actAsMirror)
        fragColor = vec4(1.0, 1.0, 1.0, a_alpha);
    else
        fragColor = vec4(v_color.xyz, a_alpha);//vec4(0.7, 0.7, 0.7, a_alpha);
}
