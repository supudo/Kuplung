#version 410 core

uniform bool has_texture;
uniform sampler2D sampler_texture;

struct LightSource_Directional {
    bool inUse;
    vec3 direction;
    vec3 ambient, diffuse, specular;
    float strengthAmbient, strengthDiffuse, strengthSpecular;
};

uniform LightSource_Directional solidSkin_Light;
uniform vec3 fs_UIAmbient;
uniform vec3 fs_cameraPosition;

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal0;
in vec3 fs_vertexNormal;
in vec3 fs_tangent0;
in vec3 fs_tangent;
in vec3 fs_bitangent0;
in vec3 fs_bitangent;

out vec4 fragColor;

void main(void) {
    vec4 processedColor = vec4(0, 0, 0, 1);
    if (has_texture)
        processedColor = texture(sampler_texture, fs_textureCoord);

    vec3 directionView = normalize(fs_cameraPosition - fs_vertexPosition);
    vec3 directionLight = normalize(-1.0 * solidSkin_Light.direction);
    float lambertFactor = max(dot(fs_vertexNormal, -directionLight), 0.0);
    vec3 directionReflection = normalize(reflect(-directionLight, fs_vertexNormal));
    float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), 1.0);
    vec3 solidLightColor = solidSkin_Light.strengthDiffuse * solidSkin_Light.diffuse * lambertFactor * processedColor.rgb;

    solidLightColor += fs_UIAmbient;

    fragColor = vec4(solidLightColor, 1.0);
}
