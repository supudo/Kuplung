#version 410 core

uniform mat4 vs_MVPMatrix;
uniform mat4 vs_WorldMatrix;

uniform vec3 vs_displacementLocation;
uniform float vs_isBorder;

uniform sampler2D vs_materialDisplacementMap;

in vec3 vs_vertexPosition;
in vec2 vs_textureCoord;
in vec3 vs_vertexNormal;
in vec3 vs_tangent;
in vec3 vs_bitangent;

out vec3 tcs_vertexPosition;
out vec2 tcs_textureCoord;
out vec3 tcs_vertexNormal0;
out vec3 tcs_vertexNormal;
out vec3 tcs_tangent0;
out vec3 tcs_tangent;
out vec3 tcs_bitangent0;
out vec3 tcs_bitangent;
out vec3 tcs_displacementLocation;
out float tcs_isBorder;

void main(void) {
    tcs_vertexPosition = vs_vertexPosition;
    tcs_textureCoord = vs_textureCoord;
    tcs_vertexNormal0 = vs_vertexNormal;
    tcs_vertexNormal = (vs_WorldMatrix * vec4(vs_vertexNormal, 0.0)).xyz;
    tcs_tangent0 = vs_tangent;
    tcs_tangent = (vs_WorldMatrix * vec4(vs_tangent, 0.0)).xyz;
    tcs_bitangent0 = vs_bitangent;
    tcs_bitangent = (vs_WorldMatrix * vec4(vs_bitangent, 0.0)).xyz;
    tcs_displacementLocation = vs_displacementLocation;
    tcs_isBorder = vs_isBorder;

    gl_Position = vs_MVPMatrix * vec4(vs_vertexPosition, 1.0);

//    vec4 dv = texture(vs_materialDisplacementMap, vs_textureCoord);
//    float df = 0.30 * dv.x + 0.59 * dv.y + 0.11 * dv.z;
//    vec4 newVertexPos = vec4(vs_vertexNormal * df * 100.0, 0.0) + vec4(vs_vertexPosition, 1.0);
//    gl_Position = vs_MVPMatrix * newVertexPos;
}
