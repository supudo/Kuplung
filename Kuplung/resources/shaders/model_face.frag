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

struct LightSource {
    bool inUse;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float strengthAmbient;
    float strengthDiffuse;
    float strengthSpecular;

    vec3 position;
    vec3 direction;
};

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal;
in vec3 fs_tangent;
in float fs_isBorder;

out vec4 fragColor;

#define NR_POINT_LIGHTS 8

uniform LightSource directionalLights[NR_POINT_LIGHTS];
uniform ModelMaterial material;

// =================================================
//
// definitions
//
// =================================================

vec3 calculateAmbient();
vec3 calculateDiffuse(vec3 normalDirection, vec3 lightDirection);
vec3 calculateSpecular(vec3 normalDirection, vec3 viewDirection);
vec3 calculateRefraction(vec3 normalDirection, vec4 texturedColor_Diffuse);
float stepmix(float edge0, float edge1, float E, float x);
vec4 celShadingColor();
vec3 calculateBumpedNormal();

// =================================================
//
// main()
//
// =================================================

void main(void) {
    if (fs_isBorder > 0.0)
        fragColor = vec4(fs_outlineColor, 1.0);
    else {
        vec4 texturedColor_Diffuse = texture(material.sampler_diffuse, fs_textureCoord);
        vec3 processedColor_Diffuse = material.has_texture_diffuse ? texturedColor_Diffuse.rgb : material.diffuse;

        vec3 fragmentNormal;
        if (material.has_texture_bump)
            fragmentNormal = calculateBumpedNormal();
        else
            fragmentNormal = fs_vertexNormal;

        // misc
        vec3 normalDirection = normalize(fragmentNormal);
        vec3 viewDirection = normalize(fs_cameraPosition - fs_vertexPosition);
        //vec3 lightDirection = normalize(directionalLights[0].position - fs_vertexPosition);
        vec3 lightDirection;
        for (int i=0; i<NR_POINT_LIGHTS; i++)
            if (directionalLights[i].inUse)
                lightDirection += normalize(directionalLights[i].position - fs_vertexPosition);

        // Ambient
        vec3 ambient = calculateAmbient();

        // Diffuse
        vec3 diffuse = calculateDiffuse(normalDirection, lightDirection);

        // Specular
        vec3 specular = calculateSpecular(normalDirection, viewDirection);

        // Refraction
        vec3 processedColorRefraction = (material.emission + ambient + diffuse + specular);
        if (material.refraction > 1.0) {
            vec3 refraction = calculateRefraction(normalDirection, texturedColor_Diffuse);
            processedColorRefraction = processedColorRefraction * refraction;
        }
        else
            processedColorRefraction = processedColorRefraction * processedColor_Diffuse;

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

    vec3 vertexTangent = normalize(fs_tangent);
    vertexTangent = normalize(vertexTangent - dot(vertexTangent, vertexNormal) * vertexNormal);

    vec3 vertexBitangent = cross(vertexTangent, vertexNormal);

    vec3 vertexBumpMapNormal = texture(material.sampler_bump, fs_textureCoord).xyz;
    vertexBumpMapNormal = 2.0 * vertexBumpMapNormal - vec3(1.0, 1.0, 1.0);

    vec3 vertexNewNormal;
    mat3 TBN = mat3(vertexTangent, vertexBitangent, vertexNormal);
    vertexNewNormal = TBN * vertexBumpMapNormal;
    vertexNewNormal = normalize(vertexNewNormal);

    return vertexNewNormal;
}

// =================================================
//
// Ambient color
//
// =================================================

vec3 calculateAmbient() {
    vec3 result;
    vec3 materialAmbient;
    if (material.has_texture_ambient)
        materialAmbient = texture(material.sampler_ambient, fs_textureCoord).rgb;
    else
        materialAmbient = material.ambient;
    for (int i=0; i<NR_POINT_LIGHTS; i++)
        if (directionalLights[i].inUse)
            result += directionalLights[i].strengthAmbient * directionalLights[i].ambient * materialAmbient;
    return result;
}

// =================================================
//
// Diffuse color
//
// =================================================

vec3 calculateDiffuse(vec3 normalDirection, vec3 lightDirection) {
    vec3 result;
    float lambertFactor = max(dot(lightDirection, normalDirection), 0.0);
    vec3 materialDiffuse;
    if (material.has_texture_diffuse)
        materialDiffuse = texture(material.sampler_diffuse, fs_textureCoord).rgb;
    else
        materialDiffuse = material.diffuse;
    for (int i=0; i<NR_POINT_LIGHTS; i++)
        if (directionalLights[i].inUse)
            result += directionalLights[i].strengthDiffuse * lambertFactor * directionalLights[i].diffuse * materialDiffuse;
    return result;
}

// =================================================
//
// Specular color
//
// =================================================

vec3 calculateSpecular(vec3 normalDirection, vec3 viewDirection) {
    vec3 result;
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        if (directionalLights[i].inUse) {
            vec3 reflectDirection = reflect(-directionalLights[i].direction, normalDirection);
            float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), directionalLights[i].strengthSpecular);
            vec3 specular = directionalLights[i].strengthSpecular * spec * directionalLights[i].specular;
            if (material.has_texture_specular) {
                vec4 texturedColor_Specular = texture(material.sampler_specular, fs_textureCoord);
                result += specular * texturedColor_Specular.rgb * material.refraction * material.specularExp;
            }
            else
                result += specular * material.specular * material.refraction * material.specularExp;
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
    for (int i=0; i<NR_POINT_LIGHTS; i++)
        if (directionalLights[i].inUse)
            L += normalize(directionalLights[i].position);
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
    for (int i=0; i<NR_POINT_LIGHTS; i++)
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
