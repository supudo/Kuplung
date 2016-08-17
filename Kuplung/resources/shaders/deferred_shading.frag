#version 410 core

out vec4 fragColor;
in vec2 TexCoords;

uniform sampler2D sampler_position;
uniform sampler2D sampler_normal;
uniform sampler2D sampler_albedospec;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform int draw_mode;
uniform float ambientStrength;
uniform float gammaCoeficient;

struct LightSource_Directional {
    bool inUse;
    vec3 direction;
    vec3 ambient, diffuse, specular;
    float strengthAmbient, strengthDiffuse, strengthSpecular;
};

struct LightSource_Point {
    bool inUse;
    vec3 position;
    float constant, linear, quadratic;
    vec3 ambient, diffuse, specular;
    float strengthAmbient, strengthDiffuse, strengthSpecular;
};

struct LightSource_Spot {
    bool inUse;
    vec3 position, direction;
    float cutOff, outerCutOff;
    float constant, linear, quadratic;
    vec3 ambient, diffuse, specular;
    float strengthAmbient, strengthDiffuse, strengthSpecular;
};

#define NR_DIRECTIONAL_LIGHTS 8
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4

uniform LightSource_Directional directionalLights[NR_DIRECTIONAL_LIGHTS];
uniform LightSource_Point pointLights[NR_POINT_LIGHTS];
uniform LightSource_Spot spotLights[NR_SPOT_LIGHTS];

vec3 calculateLightDirectional(vec3 directionNormal, vec3 directionView, vec3 colorDiffuse);
vec3 calculateLightPoint(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec3 colorDiffuse);
vec3 calculateLightSpot(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec3 colorDiffuse);

void main() {
    // Retrieve data from gbuffer
    vec3 FragPos = texture(sampler_position, TexCoords).rgb;
    vec3 Normal = texture(sampler_normal, TexCoords).rgb;
    vec3 Diffuse = texture(sampler_albedospec, TexCoords).rgb;
    float Specular = texture(sampler_albedospec, TexCoords).a;
    vec3 viewDir = normalize(viewPos - FragPos);

    // directional lights color
    vec3 lightsDirectional = vec3(0.0);
    if (directionalLights.length() > 0)
        lightsDirectional = calculateLightDirectional(Normal, viewDir, Diffuse);

    // point lights color
    vec3 lightsPoint = vec3(0.0);
    if (pointLights.length() > 0)
        lightsPoint = calculateLightPoint(FragPos, Normal, viewDir, Diffuse);

    // spot lights color
    vec3 lightsSpot = vec3(0.0);
    if (spotLights.length() > 0)
        lightsSpot = calculateLightSpot(FragPos, Normal, viewDir, Diffuse);

    // Then calculate lighting as usual
    vec3 lighting = Diffuse * ambientStrength;
    for (int i=0; i<NR_LIGHTS; ++i) {
        // Calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if (distance < lights[i].Radius) {
            // Diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

            // Specular
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights[i].Color * spec * Specular;

            // Attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }
    lighting += (lightsDirectional + lightsPoint + lightsSpot);

    if (draw_mode == 1)
        fragColor = vec4(lighting, 1.0);
    else if (draw_mode == 2)
        fragColor = vec4(FragPos, 1.0);
    else if (draw_mode == 3)
        fragColor = vec4(Normal, 1.0);
    else if (draw_mode == 4)
        fragColor = vec4(Diffuse, 1.0);
    else if (draw_mode == 5)
        fragColor = vec4(vec3(Specular), 1.0);

    // gamma correction
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gammaCoeficient));
}

// =================================================
//
// Lights - Directional (Sun)
//
// =================================================

vec3 calculateLightDirectional(vec3 directionNormal, vec3 directionView, vec3 colorDiffuse) {
    vec3 result = vec3(0.0);
    for (int i=0; i<NR_DIRECTIONAL_LIGHTS; i++) {
        if (directionalLights[i].inUse) {
            vec3 directionLight = normalize(-1.0 * directionalLights[i].direction);

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, -directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = normalize(reflect(-directionLight, directionNormal));
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), 1.0);

            result += directionalLights[i].strengthDiffuse * directionalLights[i].diffuse * lambertFactor * colorDiffuse;
        }
    }
    return result;
}

// =================================================
//
// Lights - Point (Light bulb)
//
// =================================================

vec3 calculateLightPoint(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec3 colorDiffuse) {
    vec3 result = vec3(0.0);
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        if (pointLights[i].inUse) {
            vec3 directionLight = normalize(pointLights[i].position - fragmentPosition);

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = reflect(-directionLight, directionNormal);
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), 1.0);

            // Attenuation
            float lightDistance = length(pointLights[i].position - fragmentPosition);
            float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * lightDistance + pointLights[i].quadratic * (lightDistance * lightDistance));

            result += pointLights[i].strengthDiffuse * pointLights[i].diffuse * lambertFactor * attenuation * colorDiffuse;
        }
    }
    return result;
}

// =================================================
//
// Lights - Spot (Flashlight)
//
// =================================================

vec3 calculateLightSpot(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec3 colorDiffuse) {
    vec3 result = vec3(0.0);
    for (int i=0; i<NR_SPOT_LIGHTS; i++) {
        if (spotLights[i].inUse) {
            vec3 directionLight = normalize(spotLights[i].position - fragmentPosition);

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = reflect(-directionLight, directionNormal);
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), 1.0);

            // Attenuation
            float lightDistance = length(spotLights[i].position - fragmentPosition);
            float attenuation = 1.0f / (spotLights[i].constant + spotLights[i].linear * lightDistance + spotLights[i].quadratic * (lightDistance * lightDistance));

            // Spotlight intensity
            float theta = dot(directionLight, normalize(-spotLights[i].direction));
            float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
            float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

            result += spotLights[i].strengthDiffuse * spotLights[i].diffuse * lambertFactor * attenuation * intensity * colorDiffuse;
        }
    }
    return result;
}
