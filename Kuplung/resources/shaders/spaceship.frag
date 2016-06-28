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

uniform LightSource solidSkin_Light;
uniform vec3 solidSkin_materialColor;

vec3 calculateLight(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);

out vec4 fragColor;

void main(void) {
    vec4 processedColor;
    if (has_texture)
        processedColor = texture(sampler_texture, v_textureCoord);
    else
        processedColor = v_color;

    vec3 fragmentNormal = normalize(v_vertexNormal);
    vec3 viewDirection = normalize(fs_cameraPosition - v_vertexPosition);
    vec3 lightColor = calculateLight(v_vertexNormal, viewDirection, vec4(solidSkin_materialColor, 1.0), processedColor, vec4(solidSkin_materialColor, 1.0));

    fragColor = vec4(processedColor.rgb * lightColor, 1.0);
}

vec3 calculateLight(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 directionLight = normalize(-1.0 * solidSkin_Light.direction);

    // Diffuse shading - lambert factor
    float lambertFactor = max(dot(directionNormal, -directionLight), 0.0);

    // Specular shading
    float materialRefraction = 1.0;
    vec3 directionReflection = normalize(reflect(-directionLight, directionNormal));
    float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), materialRefraction);

    // Combine results
    vec3 ambient = solidSkin_Light.strengthAmbient * solidSkin_Light.ambient * colorAmbient.rgb;
    vec3 diffuse = solidSkin_Light.strengthDiffuse * solidSkin_Light.diffuse * lambertFactor * colorDiffuse.rgb;
    vec3 specular = solidSkin_Light.strengthSpecular * solidSkin_Light.specular * specularFactor * colorSpecular.rgb;

    return ambient + diffuse + specular;
}
