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
    vec3 eyeSpaceNormal = mat3(fs_ModelMatrix) * fs_vertexNormal;

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

// =================================================
//
// Depth Testing
//
// =================================================

float linearizeDepth(float depth) {
    float z = depth * 2.0 - fs_planeClose;
    return (2.0 * fs_planeClose * fs_planeFar) / (fs_planeFar + fs_planeClose - z * (fs_planeFar - fs_planeClose));
}
