#version 410

uniform mat4 vs_MVPMatrix;
uniform mat4 vs_WorldMatrix;

uniform vec3 vs_displacementLocation;
uniform float vs_isBorder;

in vec3 vs_vertexPosition;
in vec2 vs_textureCoord;
in vec3 vs_vertexNormal;
in vec3 vs_tangent;
in vec3 vs_bitangent;

//out vec3 vso_vertexPosition;
//out vec2 vso_textureCoord;
//out vec3 vso_vertexNormal;
//out vec3 vso_displacementLocation;
//out float vso_isBorder;

out VS_OUT {
    vec3 gs_vertexPosition;
    vec2 gs_textureCoord;
    vec3 gs_vertexNormal;
    vec3 gs_tangent;
    vec3 gs_bitangent;
    vec3 gs_displacementLocation;
    float gs_isBorder;
} vs_out;

void main(void) {
//    vso_vertexPosition = vs_vertexPosition;
//    vso_textureCoord = vs_textureCoord;
//    vso_vertexNormal = vs_vertexNormal;
//    vso_displacementLocation = vs_displacementLocation;
//    vso_isBorder = vs_isBorder;

    vs_out.gs_vertexPosition = vs_vertexPosition;
    vs_out.gs_textureCoord = vs_textureCoord;
    vs_out.gs_vertexNormal = (vs_WorldMatrix * vec4(vs_vertexNormal, 0.0)).xyz;
    vs_out.gs_tangent = (vs_WorldMatrix * vec4(vs_tangent, 0.0)).xyz;
    vs_out.gs_bitangent = (vs_WorldMatrix * vec4(vs_bitangent, 0.0)).xyz;
    vs_out.gs_displacementLocation = vs_displacementLocation;
    vs_out.gs_isBorder = vs_isBorder;

    gl_Position = vs_MVPMatrix * vec4(vs_vertexPosition, 1.0);
}
