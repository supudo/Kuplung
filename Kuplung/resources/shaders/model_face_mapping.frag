// =================================================
//
// Bump map
//
// =================================================

vec3 calculateBumpedNormal(vec2 textureCoordinate) {
    vec3 vertexNewNormal = texture(material.sampler_bump, textureCoordinate).rgb;
    return normalize(vertexNewNormal * 2.0 - 1.0);
}

// =================================================
//
// Parallax map
//
// =================================================

vec2 calculateParallaxMapping(vec2 texCoords, vec3 viewDir) {
    //float height =  texture(material.sampler_displacement, texCoords).r;
    //return texCoords - viewDir.xy * (height * material.heightScale);

    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;

    // depth of current layer
    float currentLayerDepth = 0.0;

    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * material.heightScale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(material.sampler_displacement, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.sampler_displacement, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // -- parallax occlusion mapping interpolation from here on
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.sampler_displacement, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
    //return currentTexCoords;
}

mat3 cotangent_frame(vec3 normal, vec3 position, vec2 texCoords) {
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx(position);
    vec3 dp2 = dFdy(position);
    vec2 duv1 = dFdx(texCoords);
    vec2 duv2 = dFdy(texCoords);

    // solve the linear system
    vec3 dp2perp = cross(dp2, normal);
    vec3 dp1perp = cross(normal, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, normal);
}
