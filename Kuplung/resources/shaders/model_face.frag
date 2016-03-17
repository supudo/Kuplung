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
            fragmentNormal = normalize(fs_vertexNormal);

        // misc
        vec3 normalDirection = fragmentNormal;
        vec3 viewDirection = normalize(fs_cameraPosition - fs_vertexPosition);
        vec3 fragmentPosition = vec3(fs_ModelMatrix * vec4(fs_vertexPosition, 1.0f));

        // directional lights color
        vec3 lightsDirectional = calculateLightDirectional(normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // point lights color
        vec3 lightsPoint = calculateLightPoint(fragmentPosition, normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);

        // spot lights color
        vec3 lightsSpot = calculateLightSpot(fragmentPosition, normalDirection, viewDirection, processedColor_Ambient, processedColor_Diffuse, processedColor_Specular);


        // Refraction
        vec3 processedColorRefraction = (material.emission + lightsDirectional + lightsPoint + lightsSpot);
        if (effect_GBlur.gauss_mode > -1) {
            // effects - gaussian blur
            vec4 effect_GBlur_Color = effect_gaussian_blur();
            processedColorRefraction += effect_GBlur_Color.rgb;
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
