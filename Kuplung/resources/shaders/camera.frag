#version 410 core

uniform vec3 fs_color;
uniform vec3 fs_innerLightDirection;
in vec3 v_vertexNormal;
out vec4 fragColor;

void main(void) {
    vec3 lightDirection = fs_innerLightDirection;
    vec3 directionLight = normalize(lightDirection);
    float lambertFactor = max(dot(v_vertexNormal, -directionLight), 0.0);
    vec3 light = 0.6 * vec3(0.6) * lambertFactor;
    fragColor = vec4(fs_color + light, 1.0);
}
