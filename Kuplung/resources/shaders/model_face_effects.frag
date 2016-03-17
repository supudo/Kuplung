// =================================================
//
// Gaussian Blur
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
        color += texture(layer, fs_textureCoord.xy + depth * vec2(i, j) * dxy) * cw;
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
        color += texture(layer, fs_textureCoord.xy + depth * vec2(i, j) * dxy) * cw;
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
