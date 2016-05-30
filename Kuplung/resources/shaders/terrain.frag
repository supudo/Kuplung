#version 410 core

in vec3 v_vertexNormal;
in vec4 v_color;

out vec4 fragColor;

void main(void) {
    vec3 lightDirection = vec3(0.0, 0.0, 5.0);
    vec3 directionLight = normalize(lightDirection);
    float lambertFactor = max(dot(v_vertexNormal, -directionLight), 0.0);
    vec3 light = 0.6 * vec3(0.6) * lambertFactor;
    fragColor = v_color + vec4(light, 1.0);

    //fragColor = v_color;
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
