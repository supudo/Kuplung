// =================================================
//
// main()
//
// =================================================

void main(void) {
    if (fs_isBorder > 0.0)
        fragColor = vec4(fs_outlineColor, 1.0);
    else {
        vec2 textureCoords = fs_textureCoord;
        vec3 viewDirection = normalize(fs_cameraPosition - fs_vertexPosition);
        vec3 fragmentPosition = vec3(fs_ModelMatrix * vec4(fs_vertexPosition, 1.0f));
        vec3 normalDirection = fs_vertexNormal;

        // Parallax mapping coordinates
        if (fs_userParallaxMapping) {
            vec3 T = normalize(mat3(fs_ModelMatrix) * fs_tangent0);
            vec3 B = normalize(mat3(fs_ModelMatrix) * fs_bitangent0);
            vec3 N = normalize(mat3(fs_ModelMatrix) * fs_vertexNormal0);
            mat3 TBN = transpose(mat3(T, B, N));
            vec3 tangentViewPosition = TBN * fs_cameraPosition;
            vec3 tangentFragPosition = TBN * fs_vertexPosition;
            vec3 tangentViewDirection = normalize(tangentViewPosition - tangentFragPosition);
            textureCoords = calculateParallaxMapping(fs_textureCoord,  tangentViewDirection);

            if (textureCoords.x > 1.0 || textureCoords.y > 1.0 || textureCoords.x < 0.0 || textureCoords.y < 0.0)
                discard;
         }

        // ambient color/texture
        vec4 processedColor_Ambient = (material.has_texture_ambient ? texture(material.sampler_ambient, textureCoords) : vec4(material.ambient, 1.0));

        // diffuse color/texture
        vec4 processedColor_Diffuse = (material.has_texture_diffuse ? texture(material.sampler_diffuse, textureCoords) : vec4(material.diffuse, 1.0));

        // spoecular color/texture
        vec4 processedColor_Specular = (material.has_texture_specular ? texture(material.sampler_specular, textureCoords) : vec4(material.specular, 1.0));

        // bump map normal vector
        vec3 fragmentNormal;
        if (material.has_texture_bump)
            fragmentNormal = calculateBumpedNormal(textureCoords);
        else
            fragmentNormal = normalize(fragmentNormal);

        // directional lights color
        vec3 lightsDirectional = vec3(1.0);
        if (directionalLights.length() > 0)
            lightsDirectional = calculateLightDirectional(normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // point lights color
        vec3 lightsPoint = vec3(1.0);
        if (pointLights.length() > 0)
            lightsPoint = calculateLightPoint(fragmentPosition, normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // spot lights color
        vec3 lightsSpot = vec3(1.0);
        if (spotLights.length() > 0)
            lightsSpot = calculateLightSpot(fragmentPosition, normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // Refraction
        vec3 processedColorRefraction = (material.emission + lightsDirectional + lightsPoint + lightsSpot);
        if (effect_GBlur.gauss_mode > -1) {
            // effects - gaussian blur
            vec4 effect_GBlur_Color = effect_gaussian_blur();
            processedColorRefraction += effect_GBlur_Color.rgb;
        }
        if (effect_Bloom.doBloom) {
            // effects - bloom
            vec4 effect_Bloom_Color = effect_bloom();
            processedColorRefraction += effect_Bloom_Color.rgb;
        }
        if (material.refraction > 1.0) {
            vec3 refraction = calculateRefraction(normalDirection, processedColor_Diffuse);
            processedColorRefraction = processedColorRefraction * refraction;
        }
        else
            processedColorRefraction = processedColorRefraction * processedColor_Diffuse.rgb;

        // TODO: fix?
        //if (material.has_texture_specular)
        //    processedColorRefraction *= processedColor_Specular.rgb;

        // final color
        if (fs_celShading) // cel-shading
            fragColor = celShadingColor();
        else if (material.illumination_model == 0)
            fragColor = vec4((material.refraction > 1.0) ? processedColorRefraction : processedColor_Diffuse.rgb, fs_alpha);
        else
            fragColor = vec4(processedColorRefraction, fs_alpha);
    }
}
