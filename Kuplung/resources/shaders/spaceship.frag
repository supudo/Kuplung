#version 410 core

in vec3 v_vertexPosition;
in vec2 v_textureCoord;
in vec3 v_vertexNormal;
in vec4 v_color;

uniform bool has_texture;
uniform sampler2D sampler_texture;
uniform vec3 fs_cameraPosition;

struct LightSource {
    vec3 direction;
    vec3 ambient, diffuse, specular;
    float strengthAmbient, strengthDiffuse, strengthSpecular;
};

uniform LightSource lightObject[1];

vec3 calculateLight(vec3 directionNormal, vec3 directionView, vec4 processedColor);

out vec4 fragColor;

void main(void) {
    vec4 processedColor;
    if (has_texture)
        processedColor = texture(sampler_texture, v_textureCoord);
    else
        processedColor = v_color;

    vec3 fragmentNormal = normalize(v_vertexNormal);
    vec3 viewDirection = normalize(fs_cameraPosition - v_vertexPosition);
    vec3 lightColor = calculateLight(fragmentNormal, viewDirection, processedColor);

    fragColor = vec4(processedColor.rgb * lightColor, 1.0);
}

vec3 calculateLight(vec3 directionNormal, vec3 directionView, vec4 processedColor) {
    vec3 directionLight = normalize(-1.0 * lightObject[0].direction);
    float lambertFactor = max(dot(directionNormal, -directionLight), 0.0);
    vec3 directionReflection = normalize(reflect(-directionLight, directionNormal));
    float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), 1.0);
    return lightObject[0].strengthDiffuse * lightObject[0].diffuse * lambertFactor * processedColor.rgb;
}
