#version 410 core

uniform mat4 fs_ModelMatrix;
uniform mat4 fs_WorldMatrix;
uniform vec3 fs_cameraPosition;
uniform float fs_screenResX, fs_screenResY;
uniform float fs_alpha;
uniform vec3 fs_outlineColor;
uniform vec3 fs_UIAmbient;
uniform bool fs_celShading;
uniform bool fs_userParallaxMapping;
uniform int fs_modelViewSkin;
uniform float fs_gammaCoeficient;
uniform bool fs_ACESFilmRec2020;
uniform float fs_planeClose;
uniform float fs_planeFar;
uniform bool fs_showDepthColor;

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal0;
in vec3 fs_vertexNormal;
in vec3 fs_tangent0;
in vec3 fs_tangent;
in vec3 fs_bitangent0;
in vec3 fs_bitangent;
in float fs_isBorder;

struct ModelMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;

    float refraction;
    float specularExp;
    int illumination_model;
    float heightScale;

    sampler2D sampler_ambient;
    sampler2D sampler_diffuse;
    sampler2D sampler_specular;
    sampler2D sampler_specularExp;
    sampler2D sampler_dissolve;
    sampler2D sampler_bump;
    sampler2D sampler_displacement;

    bool has_texture_ambient;
    bool has_texture_diffuse;
    bool has_texture_specular;
    bool has_texture_specularExp;
    bool has_texture_dissolve;
    bool has_texture_bump;
    bool has_texture_displacement;
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

struct Effect_GaussianBlur {
    float gauss_w;
    float gauss_radius;
    int gauss_mode;
};

struct Effect_Bloom {
    bool doBloom;
    float bloom_WeightA;
    float bloom_WeightB;
    float bloom_WeightC;
    float bloom_WeightD;
    float bloom_Vignette;
    float bloom_VignetteAtt;
};

// lights & mats
#define NR_DIRECTIONAL_LIGHTS 8
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4
uniform LightSource_Directional directionalLights[NR_DIRECTIONAL_LIGHTS];
uniform LightSource_Point pointLights[NR_POINT_LIGHTS];
uniform LightSource_Spot spotLights[NR_SPOT_LIGHTS];
uniform ModelMaterial material;

// solid skin
uniform LightSource_Directional solidSkin_Light;
uniform vec3 solidSkin_materialColor;

// effect vars
uniform Effect_GaussianBlur effect_GBlur;
uniform Effect_Bloom effect_Bloom;

// calculated vars
float diffuse_texture_width = textureSize(material.sampler_diffuse, 0).r; //texture width
float diffuse_texture_height = textureSize(material.sampler_diffuse, 0).g; //texture height
//vec2 diffuse_texel = vec2(1.0 / diffuse_texture_width, 1.0 / diffuse_texture_height);
vec2 diffuse_dxy = vec2(1.0 / max(diffuse_texture_width, diffuse_texture_height));

// shadows
uniform bool fs_showShadows;
uniform bool fs_shadowPass;
uniform bool fs_debugShadowTexture;
uniform sampler2D sampler_shadowMap;
in vec3 fs_shadow_Normal;
in vec4 fs_shadow_FragPosLightSpace;

// functions
vec3 calculateLightSolid(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);
vec3 calculateLightDirectional(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);
vec3 calculateLightPoint(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);
vec3 calculateLightSpot(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular);
vec4 celShadingColor();
vec3 calculateBumpedNormal();
vec3 calculateRefraction(vec3 normalDirection, vec4 texturedColor_Diffuse);
float stepmix(float edge0, float edge1, float E, float x);
mat3 cotangent_frame(vec3 normal, vec3 position, vec2 texCoords);
vec3 ACESFilmRec2020(vec3 x);
float calculateShadowValue(vec3 fragmentPosition);
float linearizeDepth(float depth);

// out color
out vec4 fragColor;
