uniform mat4 vs_MVPMatrix;

in vec3 vs_vertexPosition;
in vec2 vs_textureCoord;
in vec3 vs_vertexNormal;
uniform vec3 vs_displacementLocation;

//out vec3 vso_vertexPosition;
//out vec2 vso_textureCoord;
//out vec3 vso_vertexNormal;

out VS_OUT {
    vec3 gs_vertexPosition;
    vec2 gs_textureCoord;
    vec3 gs_vertexNormal;
    vec3 gs_displacementLocation;
} vs_out;

void main(void) {
//    vso_vertexPosition = vs_vertexPosition;
//    vso_textureCoord = vs_textureCoord;
//    vso_vertexNormal = vs_vertexNormal;
//    vso_displacementLocation = vs_displacementLocation;

    vs_out.gs_vertexPosition = vs_vertexPosition;
    vs_out.gs_textureCoord = vs_textureCoord;
    vs_out.gs_vertexNormal = vs_vertexNormal;
    vs_out.gs_displacementLocation = vs_displacementLocation;

    gl_Position = vs_MVPMatrix * vec4(vs_vertexPosition, 1.0);
}
