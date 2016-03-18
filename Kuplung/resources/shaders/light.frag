#version 410

uniform sampler2D u_sampler;
uniform bool fs_useColor;
uniform vec3 fs_color;
in vec2 v_textureCoord;
in vec3 v_vertexNormal;
out vec4 fragColor;

void main(void) {
    if (fs_useColor) {
        vec3 lightDirection = vec3(0.0, 0.0, 5.0);
        vec3 directionLight = normalize(lightDirection);
        float lambertFactor = max(dot(v_vertexNormal, -directionLight), 0.0);
        vec3 light = 0.7 * vec3(0.7) * lambertFactor;
        fragColor = vec4(fs_color + light, 1.0);
    }
    else
        fragColor = texture(u_sampler, v_textureCoord);
}
