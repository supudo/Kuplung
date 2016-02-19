#version 410

in vec4 v_color;

out vec4 fragColor;

void main(void) {
    fragColor = v_color;
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
