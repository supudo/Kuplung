// =================================================
//
// PBR
//
// =================================================

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 getNormalFromMap() {
    if (material.has_texture_bump) {
        vec3 tangentNormal = texture(material.sampler_bump, fs_textureCoord).xyz * 2.0 - 1.0;

        vec3 Q1 = dFdx(fs_vertexPosition);
        vec3 Q2 = dFdy(fs_vertexPosition);
        vec2 st1 = dFdx(fs_textureCoord);
        vec2 st2 = dFdy(fs_textureCoord);

        vec3 N = normalize(fs_vertexNormal);
        vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
        vec3 B = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        return normalize(TBN * tangentNormal);
    }
    else
        return normalize(fs_vertexNormal);
}

vec3 calculatePBR() {
    vec4 processedColor_Ambient = (material.has_texture_ambient ? texture(material.sampler_ambient, fs_textureCoord) : vec4(material.ambient, 1.0));
    vec4 processedColor_Diffuse = (material.has_texture_diffuse ? texture(material.sampler_diffuse, fs_textureCoord) : vec4(material.diffuse, 1.0));
    vec4 processedColor_Specular = (material.has_texture_specular ? texture(material.sampler_specular, fs_textureCoord) : vec4(material.specular, 1.0));

    vec3 albedo = pow(processedColor_Diffuse.rgb, vec3(2.2));
    float metallic = fs_PBR_Metallic;
    float roughness = fs_PBR_Roughness;
    float ao = fs_PBR_AO;
    if (ao == 0)
        ao = processedColor_Ambient.r;
    vec3 WorldPos = fs_vertexPosition;
    vec3 N = getNormalFromMap();
    vec3 V = normalize(fs_cameraPosition - fs_vertexPosition);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use their albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        if (pointLights[i].inUse) {
            // calculate per-light radiance
            vec3 L = normalize(pointLights[i].position - WorldPos);
            vec3 H = normalize(V + L);
            float distance = length(pointLights[i].position - WorldPos);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = pointLights[i].diffuse * attenuation * pointLights[i].strengthDiffuse;

            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 nominator = NDF * G * F;
            float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
            vec3 brdf = nominator / denominator;

            // kS is equal to Fresnel
            vec3 kS = F;
            // for energy conservation, the diffuse and specular light can't
            // be above 1.0 (unless the surface emits light); to preserve this
            // relationship the diffuse component (kD) should equal 1.0 - kS.
            vec3 kD = vec3(1.0) - kS;
            // multiply kD by the inverse metalness such that only non-metals
            // have diffuse lighting, or a linear blend if partly metal (pure metals
            // have no diffuse light).
            kD *= 1.0 - metallic;

            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0);

            // add to outgoing radiance Lo
            // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
            Lo += (kD * albedo / PI + brdf) * radiance * NdotL;
        }
    }

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 returnColor = ambient + Lo;

    // HDR tonemapping
    returnColor = returnColor / (returnColor + vec3(1.0));

    // gamma correct
    returnColor = pow(returnColor, vec3(1.0 / 2.2));

    return returnColor;
}
