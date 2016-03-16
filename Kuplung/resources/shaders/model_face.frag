#version 410

uniform mat4 fs_MMatrix;
uniform mat4 fs_WorldMatrix;
uniform vec3 fs_cameraPosition;
uniform float fs_screenResX, fs_screenResY;
uniform float fs_alpha;
uniform vec3 fs_outlineColor;
uniform bool fs_celShading;

struct ModelMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;

    float refraction;
    float specularExp;
    int illumination_model;

    sampler2D sampler_ambient;
    sampler2D sampler_diffuse;
    sampler2D sampler_specular;
    sampler2D sampler_specularExp;
    sampler2D sampler_dissolve;
    sampler2D sampler_bump;

    bool has_texture_ambient;
    bool has_texture_diffuse;
    bool has_texture_specular;
    bool has_texture_specularExp;
    bool has_texture_dissolve;
    bool has_texture_bump;
};

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
};

struct LightSource_Spot {
    bool inUse;
    vec3 position, direction;
    float cutOff, outerCutOff;
    float constant, linear, quadratic;
    vec3 ambient, diffuse, specular;
};

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal;
in vec3 fs_tangent;
in vec3 fs_bitangent;
in float fs_isBorder;

out vec4 fragColor;

#define NR_DIRECTIONAL_LIGHTS 8
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4

uniform LightSource_Directional directionalLights[NR_DIRECTIONAL_LIGHTS];
uniform LightSource_Point pointLights[NR_POINT_LIGHTS];
uniform LightSource_Spot spotLights[NR_SPOT_LIGHTS];
uniform ModelMaterial material;

// =================================================
//
// definitions
//
// =================================================

vec3 calculateLightDirectional(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);
vec3 calculateLightPoint(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);
vec3 calculateLightSpot(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);

vec4 celShadingColor();
vec3 calculateBumpedNormal();

vec3 calculateRefraction(vec3 normalDirection, vec4 texturedColor_Diffuse);
float stepmix(float edge0, float edge1, float E, float x);

// =================================================
//
// main()
//
// =================================================

void main(void) {
    if (fs_isBorder > 0.0)
        fragColor = vec4(fs_outlineColor, 1.0);
    else {
        // ambient color/texture
        vec4 processedColor_Ambient = (material.has_texture_ambient ? texture(material.sampler_ambient, fs_textureCoord) : vec4(material.ambient, 1.0));

        // diffuse color/texture
        vec4 processedColor_Diffuse = (material.has_texture_diffuse ? texture(material.sampler_diffuse, fs_textureCoord) : vec4(material.diffuse, 1.0));

        // spoecular color/texture
        vec4 processedColor_Specular = (material.has_texture_specular ? texture(material.sampler_specular, fs_textureCoord) : vec4(material.specular, 1.0));

        // bump map normal vector
        vec3 fragmentNormal;
        if (material.has_texture_bump)
            fragmentNormal = calculateBumpedNormal();
        else
            fragmentNormal = fs_vertexNormal;

        // misc
        vec3 normalDirection = normalize(fragmentNormal);
        vec3 viewDirection = normalize(fs_cameraPosition - fs_vertexPosition);

        // directional lights color
        vec3 lightsDirectional = calculateLightDirectional(normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // point lights color
        vec3 lightsPoint = calculateLightPoint(normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // spot lights color
        vec3 lightsSpot = calculateLightSpot(normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // Refraction
        vec3 processedColorRefraction = (material.emission + lightsDirectional + lightsPoint + lightsSpot);
        if (material.refraction > 1.0) {
            vec3 refraction = calculateRefraction(normalDirection, processedColor_Diffuse);
            processedColorRefraction = processedColorRefraction * refraction;
        }
        else
            processedColorRefraction = processedColorRefraction * processedColor_Diffuse.rgb;

        // TODO: fix?
        if (material.has_texture_specular)
            processedColorRefraction *= processedColor_Specular.rgb;

        // final color
        if (fs_celShading) // cel-shading
            fragColor = celShadingColor();
        else {
            if (material.illumination_model == 0)
                fragColor = vec4((material.refraction > 1.0) ? processedColorRefraction : processedColor_Diffuse.rgb, fs_alpha);
            else
                fragColor = vec4(processedColorRefraction, fs_alpha);
        }
    }
}

// =================================================
//
// Bump map
//
// =================================================

vec3 calculateBumpedNormal() {
    vec3 vertexNormal = normalize(fs_vertexNormal);

    mat3 mtxMV = mat3(fs_MMatrix);

    vec3 vertexTangent = normalize(fs_tangent);
    vertexTangent = normalize(vertexTangent - dot(vertexTangent, vertexNormal) * vertexNormal);

    vec3 vertexBitangent = cross(vertexTangent, vertexNormal);

    //vec3 vertexBumpMapNormal = texture(material.sampler_bump, vec2(fs_textureCoord.x, -fs_textureCoord.y)).rgb;
    vec3 vertexBumpMapNormal = texture(material.sampler_bump, fs_textureCoord).rgb;
    vertexBumpMapNormal = 2.0 * vertexBumpMapNormal - vec3(1.0, 1.0, 1.0);

    vec3 vertexNewNormal;
    mat3 TBN = mat3(vertexTangent, vertexBitangent, vertexNormal);
    vertexNewNormal = TBN * vertexBumpMapNormal;
    vertexNewNormal = normalize(vertexNewNormal);

    return vertexNewNormal;
}

// =================================================
//
// Lights - Directional (Sun)
//
// =================================================

vec3 calculateLightDirectional(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 result;
    for (int i=0; i<NR_DIRECTIONAL_LIGHTS; i++) {
        if (directionalLights[i].inUse) {
            vec3 directionLight = normalize(directionalLights[i].direction);

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = reflect(-directionLight, directionNormal);
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), material.refraction);

            // Combine results
            vec3 ambient = directionalLights[i].strengthAmbient * directionalLights[i].ambient * colorAmbient.rgb;
            vec3 diffuse = directionalLights[i].strengthDiffuse * directionalLights[i].diffuse * lambertFactor * colorDiffuse.rgb;
            vec3 specular = directionalLights[i].strengthSpecular * directionalLights[i].specular * specularFactor * colorSpecular.rgb;

            result += ambient + diffuse + specular;
        }
    }
    return result;
}

// =================================================
//
// Lights - Point (Light bulb)
//
// =================================================

vec3 calculateLightPoint(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 result;
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        if (pointLights[i].inUse) {
            vec3 directionLight = normalize(pointLights[i].position - fs_vertexPosition);
            //vec3 directionLight = pointLights[i].position;

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = reflect(-directionLight, directionNormal);
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), material.refraction);

            // Attenuation
            float lightDistance = length(pointLights[i].position - fs_vertexPosition);
            float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * lightDistance + pointLights[i].quadratic * (lightDistance * lightDistance));

            // Combine results
            vec3 ambient = pointLights[i].ambient * attenuation * colorAmbient.rgb;
            vec3 diffuse = pointLights[i].diffuse * lambertFactor * attenuation * colorDiffuse.rgb;
            vec3 specular = pointLights[i].specular * specularFactor * attenuation * colorSpecular.rgb;

            result += ambient + diffuse + specular;
        }
    }
    return result;
}

// =================================================
//
// Lights - Spot (Flashlight)
//
// =================================================

vec3 calculateLightSpot(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 result;
    for (int i=0; i<NR_SPOT_LIGHTS; i++) {
        if (spotLights[i].inUse) {
            vec3 directionLight = normalize(spotLights[i].position - fs_vertexPosition);

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = reflect(-directionLight, directionNormal);
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), material.refraction);

            // Attenuation
            float lightDistance = length(spotLights[i].position - fs_vertexPosition);
            float attenuation = 1.0f / (spotLights[i].constant + spotLights[i].linear * lightDistance + spotLights[i].quadratic * (lightDistance * lightDistance));

            // Spotlight intensity
            float theta = dot(directionLight, normalize(-spotLights[i].direction));
            float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
            float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

            // Combine results
            vec3 ambient = spotLights[i].ambient * attenuation * intensity * colorAmbient.rgb;
            vec3 diffuse = spotLights[i].diffuse * lambertFactor * attenuation * intensity * colorDiffuse.rgb;
            vec3 specular = spotLights[i].specular * specularFactor * attenuation * intensity * colorSpecular.rgb;

            result += ambient + diffuse + specular;
        }
    }
    return result;
}

// =================================================
//
// Refraction
//
// =================================================

vec3 calculateRefraction(vec3 normalDirection, vec4 texturedColor_Diffuse) {
    vec3 refraction_normal = normalize(refract(fs_vertexPosition, normalDirection, material.refraction));
    vec3 refractionColor = mix(texture(material.sampler_diffuse, fs_textureCoord + refraction_normal.xy * 0.1), texturedColor_Diffuse, fs_alpha).rgb;
    vec2 pixelTexCoords = vec2(gl_FragCoord.x / fs_screenResX, gl_FragCoord.y / fs_screenResY);
    vec3 refraction = texture(material.sampler_diffuse, pixelTexCoords + refraction_normal.xy * 0.1).rgb;
    return refraction;
}

// =================================================
//
// CelShading
//
// =================================================

vec4 celShadingColor() {
    vec3 eyeSpaceNormal = mat3(fs_MMatrix) * fs_vertexNormal;

    vec3 N = normalize(eyeSpaceNormal);
    vec3 L;
    for (int i=0; i<NR_DIRECTIONAL_LIGHTS; i++)
        if (directionalLights[i].inUse)
            L += normalize(directionalLights[i].direction);
    vec3 Eye = vec3(0, 0, 1);
    vec3 H = normalize(L + Eye);

    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, material.refraction);

    const float A = 0.1;
    const float B = 0.3;
    const float C = 0.6;
    const float D = 1.0;
    float E = fwidth(df);

    if (df > A - E && df < A + E) df = stepmix(A, B, E, df);
    else if (df > B - E && df < B + E) df = stepmix(B, C, E, df);
    else if (df > C - E && df < C + E) df = stepmix(C, D, E, df);
    else if (df < A) df = 0.0;
    else if (df < B) df = B;
    else if (df < C) df = C;
    else df = D;

    E = fwidth(sf);
    if (sf > 0.5 - E && sf < 0.5 + E)
        sf = smoothstep(0.5 - E, 0.5 + E, sf);
    else
        sf = step(0.5, sf);

    vec3 celAmbient;
    for (int i=0; i<NR_DIRECTIONAL_LIGHTS; i++)
        if (directionalLights[i].inUse)
            celAmbient += material.ambient * directionalLights[i].specular * directionalLights[i].strengthSpecular;
    vec3 celDiffuse = df * material.diffuse;// * directionalLights[0].diffuse * directionalLights[0].strengthDiffuse;
    vec3 celSpecular = sf * material.specular;// * directionalLights[0].specular * directionalLights[0].strengthSpecular;

    vec3 color = celAmbient + celDiffuse + celSpecular;

    return vec4(color, fs_alpha);
}

float stepmix(float edge0, float edge1, float E, float x) {
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}
