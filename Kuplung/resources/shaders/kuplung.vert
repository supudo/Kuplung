uniform mat4 vs_MVPMatrix;

in vec3 vs_vertexPosition;
in vec2 vs_textureCoord;
in vec3 vs_vertexNormal;

out VS_OUT {
    vec3 gs_vertexPosition;
    vec2 gs_textureCoord;
    vec3 gs_vertexNormal;
} vs_out;

void main(void) {
    vs_out.gs_vertexPosition = vs_vertexPosition;
    vs_out.gs_textureCoord = vs_textureCoord;
    vs_out.gs_vertexNormal = vs_vertexNormal;

    gl_Position = vs_MVPMatrix * vec4(vs_vertexPosition, 1.0);
}
