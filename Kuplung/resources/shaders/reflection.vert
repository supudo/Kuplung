#version 410

uniform mat4 vs_modelViewMatrix;
uniform mat4 vs_modelViewProjectionMatrix;
uniform mat3 vs_normalMatrix;

in vec3 vs_inNormal;
in vec4 vs_inPosition;

out vec3 fs_normal;
out vec3 fs_eyeDirection;

void main (void) {
    gl_Position	= vs_modelViewProjectionMatrix * vs_inPosition;

    vec4 eyePos = vs_modelViewMatrix * vs_inPosition;
    fs_normal = normalize(vs_normalMatrix * vs_inNormal);
    fs_eyeDirection = eyePos.xyz;
}
