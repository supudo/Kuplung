#version 410 core

layout (location = 0) in vec3 vs_vertexPosition;
layout (location = 1) in vec3 vs_vertexNormal;
layout (location = 2) in vec2 vs_textureCoord;
layout (location = 3) in vec3 vs_tangent;
layout (location = 4) in vec3 vs_bitangent;

uniform mat4 vs_MVPMatrix;
uniform mat4 vs_WorldMatrix;

out vec3 tcs_vertexPosition;
out vec2 tcs_textureCoord;
out vec3 tcs_vertexNormal0;
out vec3 tcs_vertexNormal;
out vec3 tcs_tangent0;
out vec3 tcs_tangent;
out vec3 tcs_bitangent0;
out vec3 tcs_bitangent;

void main(void) {
    tcs_vertexPosition = vs_vertexPosition;
    tcs_textureCoord = vs_textureCoord;
    tcs_vertexNormal0 = vs_vertexNormal;
    tcs_vertexNormal = (vs_WorldMatrix * vec4(vs_vertexNormal, 0.0)).xyz;
    tcs_tangent0 = vs_tangent;
    tcs_tangent = (vs_WorldMatrix * vec4(vs_tangent, 0.0)).xyz;
    tcs_bitangent0 = vs_bitangent;
    tcs_bitangent = (vs_WorldMatrix * vec4(vs_bitangent, 0.0)).xyz;

    gl_Position = vs_MVPMatrix * vec4(vs_vertexPosition, 1.0);
}
