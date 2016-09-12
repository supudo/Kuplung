// =================================================
//
// Filmic tone maping
// see https://knarkowicz.wordpress.com/2016/08/31/hdr-display-first-steps/
//
// =================================================

vec3 ACESFilmRec2020(vec3 x) {
    float a = 15.8f;
    float b = 2.12f;
    float c = 1.2f;
    float d = 5.92f;
    float e = 1.9f;
    return vec3( x * ( a * x + b ) ) / ( x * ( c * x + d ) + e );
}

// =================================================
//
// Gaussian Blur
// see https://github.com/kmkolasinski/AwesomeBump/blob/master/Sources/resources/filters_3d.frag
//
// =================================================

float effect_gaussian(vec2 pos, float w) {
    return exp((-(pos.x) * (pos.x) - (pos.y) * (pos.y)) / (w * w + 1.0));
}

vec4 effect_gauss_filter_h(sampler2D layer, float w, int radius, float depth) {
    vec4 color  = vec4(0);
    float totalw = 0;
    int i = 0;
    for (int j = -radius ; j <= radius ; j++) {
        float cw = effect_gaussian(vec2(i, j), w);
        totalw += cw;
        color += texture(layer, fs_textureCoord.xy + depth * vec2(i, j) * diffuse_dxy) * cw;
    }
    return color / totalw;
}

vec4 effect_gauss_filter_v(sampler2D layer, float w, int radius, float depth) {
    vec4 color = vec4(0);
    float totalw = 0;
    int j = 0;
    for (int i = -radius ; i <= radius ; i++) {
        float cw = effect_gaussian(vec2(i, j), w);
        totalw += cw;
        color += texture(layer, fs_textureCoord.xy + depth * vec2(i, j) * diffuse_dxy) * cw;
    }
    return color / totalw;
}

vec4 effect_gaussian_blur() {
    float w = effect_GBlur.gauss_w;
    float depth = 2.0;
    int radius = int(effect_GBlur.gauss_radius);
    if (effect_GBlur.gauss_mode == 0)
        return effect_gauss_filter_h(material.sampler_diffuse, w, radius, depth);
    else
        return effect_gauss_filter_v(material.sampler_diffuse, w, radius, depth);
}

// =================================================
//
// Bloom
//
// =================================================

vec4 effect_bloom() {
   vec3 color = texture(material.sampler_diffuse, fs_textureCoord.st).rgb;
   vec3 b1 = texture(material.sampler_ambient, fs_textureCoord.st).rgb * effect_Bloom.bloom_WeightA;
   vec3 b2 = texture(material.sampler_specular, fs_textureCoord.st).rgb * effect_Bloom.bloom_WeightB;
   vec3 b3 = texture(material.sampler_specularExp, fs_textureCoord.st).rgb * effect_Bloom.bloom_WeightC;
   vec3 b4 = texture(material.sampler_dissolve, fs_textureCoord.st).rgb * effect_Bloom.bloom_WeightD;

   vec3 bloom =  (b1 + b2 + b3 + b4);
   float x = fs_textureCoord.x;
   float y = fs_textureCoord.y;
   float attenuateX = 1.0 - effect_Bloom.bloom_VignetteAtt * (1 - smoothstep(0.0, effect_Bloom.bloom_Vignette, x) + smoothstep(1 - effect_Bloom.bloom_Vignette, 1.0, x));
   float attenuateY = 1.0 - effect_Bloom.bloom_VignetteAtt * (1 - smoothstep(0.0, effect_Bloom.bloom_Vignette, y) + smoothstep(1 - effect_Bloom.bloom_Vignette, 1.0, y));

   return vec4(color + bloom * attenuateX * attenuateY, 1);
}
