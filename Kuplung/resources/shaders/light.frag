uniform sampler2D u_sampler;
in vec2 v_textureCoord;
out vec4 fragColor;

void main(void) {
    fragColor = texture(u_sampler, v_textureCoord);
}
