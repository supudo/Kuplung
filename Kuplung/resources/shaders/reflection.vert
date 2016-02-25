#version 410

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;

in vec3 inNormal;
in vec4 inPosition;
out vec3 varNormal;
out vec3 varEyeDir;

void main (void) {
    gl_Position	= modelViewProjectionMatrix * inPosition;

    vec4 eyePos = modelViewMatrix * inPosition;
    varNormal = normalize(normalMatrix * inNormal);
    varEyeDir = eyePos.xyz;
}
