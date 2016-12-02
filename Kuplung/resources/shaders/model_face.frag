// =================================================
//
// main()
//
// =================================================

void main(void) {
    if (fs_shadowPass) {
        vec4 processedColor_Ambient = (material.has_texture_ambient ? texture(material.sampler_ambient, fs_textureCoord) : vec4(material.ambient, 1.0));
        vec4 processedColor_Diffuse = (material.has_texture_diffuse ? texture(material.sampler_diffuse, fs_textureCoord) : vec4(material.diffuse, 1.0));
        vec4 processedColor_Specular = (material.has_texture_specular ? texture(material.sampler_specular, fs_textureCoord) : vec4(material.specular, 1.0));
        vec3 fragmentPosition = vec3(fs_ModelMatrix * vec4(fs_vertexPosition, 1.0f));
        fragColor = (processedColor_Ambient + (1.0 - calculateShadowValue(fragmentPosition)) * (processedColor_Diffuse + processedColor_Specular));
    }
    else {
        if (fs_isBorder > 0.0)
            fragColor = vec4(fs_outlineColor, 1.0);
        else {
            vec3 viewDirection = normalize(fs_cameraPosition - fs_vertexPosition);
            vec3 normalDirection = fs_vertexNormal;
            vec2 textureCoords = fs_textureCoord;

            // bump map normal vector
            vec3 fragmentNormal;
            if (material.has_texture_bump)
                fragmentNormal = calculateBumpedNormal(textureCoords);
            else
                fragmentNormal = normalize(normalDirection);

            if (fs_modelViewSkin == 0) { // solid
                vec3 solidLightColor = calculateLightSolid(normalDirection, viewDirection, vec4(solidSkin_materialColor, 1.0), vec4(solidSkin_materialColor, 1.0), vec4(solidSkin_materialColor, 1.0));
                solidLightColor += fs_UIAmbient;
                fragColor = vec4(solidLightColor, fs_alpha);
            }
            else if (fs_modelViewSkin == 1) { // material
                vec4 processedColor_Ambient = vec4(material.ambient, 1.0);
                vec4 processedColor_Diffuse = vec4(material.diffuse, 1.0);
                vec4 processedColor_Specular = vec4(material.specular, 1.0);
                vec3 solidLightColor = calculateLightSolid(normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);
                solidLightColor += fs_UIAmbient;
                fragColor = vec4(solidLightColor, fs_alpha);
            }
            else if (fs_modelViewSkin == 2) { // texture
                vec4 processedColor_Ambient = (material.has_texture_ambient ? texture(material.sampler_ambient, textureCoords) : vec4(solidSkin_materialColor, 1.0));
                vec4 processedColor_Diffuse = (material.has_texture_diffuse ? texture(material.sampler_diffuse, textureCoords) : vec4(solidSkin_materialColor, 1.0));
                vec4 processedColor_Specular = (material.has_texture_specular ? texture(material.sampler_specular, textureCoords) : vec4(solidSkin_materialColor, 1.0));
                vec3 solidLightColor = calculateLightSolid(fragmentNormal, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);
                fragColor = vec4(solidLightColor, fs_alpha);
            }
            else if (fs_modelViewSkin == 3) // wireframe
                fragColor = vec4(1.0);
            else if (fs_modelViewSkin == 4) { // full render
                // ambient color/texture
                vec4 processedColor_Ambient = (material.has_texture_ambient ? texture(material.sampler_ambient, textureCoords) : vec4(material.ambient, 1.0));

                // diffuse color/texture
                vec4 processedColor_Diffuse = (material.has_texture_diffuse ? texture(material.sampler_diffuse, textureCoords) : vec4(material.diffuse, 1.0));

                // specular color/texture
                vec4 processedColor_Specular = (material.has_texture_specular ? texture(material.sampler_specular, textureCoords) : vec4(material.specular, 1.0));

                // fragment position
                vec3 fragmentPosition = vec3(fs_ModelMatrix * vec4(fs_vertexPosition, 1.0f));

                // Parallax mapping coordinates
                if (fs_userParallaxMapping) {
                    mat3 TBN = cotangent_frame(normalDirection, fragmentPosition, textureCoords);
                    vec3 tangentViewPosition = TBN * fs_cameraPosition;
                    fragmentPosition = TBN * fragmentPosition;
                    vec3 tangentViewDirection = normalize(tangentViewPosition - fragmentPosition);
                    textureCoords = calculateParallaxMapping(fs_textureCoord,  tangentViewDirection);

                    if (textureCoords.x > 1.0 || textureCoords.y > 1.0 || textureCoords.x < 0.0 || textureCoords.y < 0.0)
                        discard;
                 }

                // directional lights color
                vec3 lightsDirectional = vec3(0.0);
                if (directionalLights.length() > 0)
                    lightsDirectional = calculateLightDirectional(fragmentNormal, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

                // point lights color
                vec3 lightsPoint = vec3(0.0);
                if (pointLights.length() > 0)
                    lightsPoint = calculateLightPoint(fragmentPosition, fragmentNormal, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

                // spot lights color
                vec3 lightsSpot = vec3(0.0);
                if (spotLights.length() > 0)
                    lightsSpot = calculateLightSpot(fragmentPosition, fragmentNormal, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

                // Refraction
                vec3 processedColorRefraction = (material.emission + lightsDirectional) + (lightsPoint + lightsSpot) + fs_UIAmbient;
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
                    vec3 refraction = calculateRefraction(fragmentNormal, processedColor_Diffuse);
                    processedColorRefraction = processedColorRefraction * refraction;
                }
                else
                    processedColorRefraction = processedColorRefraction * processedColor_Diffuse.rgb;

                // TODO: fix?
    //            if (material.has_texture_specular)
    //                processedColorRefraction *= processedColor_Specular.rgb;

                // final color
                if (fs_celShading) // cel-shading
                    fragColor = celShadingColor();
                else if (material.illumination_model == 0)
                    fragColor = vec4((material.refraction > 1.0) ? processedColorRefraction : processedColor_Diffuse.rgb, fs_alpha);
                else
                    fragColor = vec4(processedColorRefraction, fs_alpha);

                // tone mapping
                if (fs_ACESFilmRec2020)
                    fragColor.rgb = ACESFilmRec2020(fragColor.rgb);

                // shadows
                if (fs_showShadows)
                    fragColor = (processedColor_Ambient + (1.0 - calculateShadowValue(fragmentPosition)) * (processedColor_Diffuse + processedColor_Specular)) * fragColor;

                fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / fs_gammaCoeficient));

                if (fs_debugShadowTexture)
                    fragColor = texture(sampler_shadowMap, textureCoords);
            }
            else
                fragColor = vec4(0.7, 0.7, 0.7, fs_alpha);
        }

        if (fs_showDepthColor) {
            float depth = linearizeDepth(gl_FragCoord.z) / fs_planeFar;
            fragColor = vec4(vec3(depth), 1.0f);
        }
    }
}
