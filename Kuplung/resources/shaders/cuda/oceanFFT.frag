#version 410 core

// GLSL fragment shader
in vec3 v_eyeSpacePos; // eyeSpacePos
in vec3 v_eyeSpaceNormal; // worldSpaceNormal;
in vec3 v_worldSpaceNormal; // eyeSpaceNormal;

uniform vec4 deepColor;
uniform vec4 shallowColor;
uniform vec4 skyColor;
uniform vec3 lightDir;

out vec4 fragColor;

void main() {
    vec3 eyeVector = normalize(v_eyeSpacePos);
    vec3 eyeSpaceNormalVector = normalize(v_eyeSpaceNormal);
    vec3 worldSpaceNormalVector = normalize(v_worldSpaceNormal);

    float facing = max(0.0, dot(eyeSpaceNormalVector, -eyeVector));
    float fresnel = pow(1.0 - facing, 5.0);
    float diffuse = max(0.0, dot(worldSpaceNormalVector, lightDir));

    vec4 waterColor = mix(shallowColor, deepColor, facing);
    fragColor = waterColor * diffuse + skyColor * fresnel;
}
