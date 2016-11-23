// =================================================
//
// Lights - Solid Light
//
// =================================================

vec3 calculateLightSolid(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 directionLight = normalize(-1.0 * solidSkin_Light.direction);

    // Diffuse shading - lambert factor
    float lambertFactor = max(dot(directionNormal, -directionLight), 0.0);

    // Specular shading
    float materialRefraction = 1.0;
    vec3 directionReflection = normalize(reflect(-directionLight, directionNormal));
    float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), materialRefraction);

    // Combine results
    vec3 ambient = solidSkin_Light.strengthAmbient * solidSkin_Light.ambient * colorAmbient.rgb;
    vec3 diffuse = solidSkin_Light.strengthDiffuse * solidSkin_Light.diffuse * lambertFactor * colorDiffuse.rgb;
    vec3 specular = solidSkin_Light.strengthSpecular * solidSkin_Light.specular * specularFactor * colorSpecular.rgb;

    return ambient + diffuse + specular;
}

// =================================================
//
// Lights - Directional (Sun)
//
// =================================================

vec3 calculateLightDirectional(vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 result = vec3(0.0);
    for (int i=0; i<NR_DIRECTIONAL_LIGHTS; i++) {
        if (directionalLights[i].inUse) {
            vec3 directionLight = normalize(vec3(-1.0 * directionalLights[i].direction));

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, -directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = normalize(reflect(-directionLight, directionNormal));
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), material.refraction);

            // Combine results
            vec3 ambient = directionalLights[i].strengthAmbient * directionalLights[i].ambient * colorAmbient.rgb;
            vec3 diffuse = directionalLights[i].strengthDiffuse * directionalLights[i].diffuse * lambertFactor * colorDiffuse.rgb;
            vec3 specular = directionalLights[i].strengthSpecular * directionalLights[i].specular * specularFactor * colorSpecular.rgb;

            result += ambient + diffuse + specular;
        }
    }
    return result;
}

// =================================================
//
// Lights - Point (Light bulb)
//
// =================================================

vec3 calculateLightPoint(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 result = vec3(0.0);
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        if (pointLights[i].inUse) {
            vec3 directionLight = normalize(pointLights[i].position - fragmentPosition);

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = reflect(-directionLight, directionNormal);
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), material.refraction);

            // Attenuation
            float lightDistance = length(pointLights[i].position - fragmentPosition);
            float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * lightDistance + pointLights[i].quadratic * (lightDistance * lightDistance));

            // Combine results
            vec3 ambient = pointLights[i].strengthAmbient * pointLights[i].ambient * attenuation * colorAmbient.rgb;
            vec3 diffuse = pointLights[i].strengthDiffuse * pointLights[i].diffuse * lambertFactor * attenuation * colorDiffuse.rgb;
            vec3 specular = pointLights[i].strengthSpecular * pointLights[i].specular * specularFactor * attenuation * colorSpecular.rgb;

            result += ambient + diffuse + specular;
        }
    }
    return result;
}

// =================================================
//
// Lights - Spot (Flashlight)
//
// =================================================

vec3 calculateLightSpot(vec3 fragmentPosition, vec3 directionNormal, vec3 directionView, vec4 colorAmbient, vec4 colorDiffuse, vec4 colorSpecular) {
    vec3 result = vec3(0.0);
    for (int i=0; i<NR_SPOT_LIGHTS; i++) {
        if (spotLights[i].inUse) {
            vec3 directionLight = normalize((-1.0 * spotLights[i].position) - fragmentPosition);

            // Diffuse shading - lambert factor
            float lambertFactor = max(dot(directionNormal, directionLight), 0.0);

            // Specular shading
            vec3 directionReflection = reflect(-directionLight, directionNormal);
            float specularFactor = pow(max(dot(directionView, directionReflection), 0.0), material.refraction);

            // Attenuation
            float lightDistance = length(spotLights[i].position - fragmentPosition);
            float attenuation = 1.0f / (spotLights[i].constant + spotLights[i].linear * lightDistance + spotLights[i].quadratic * (lightDistance * lightDistance));

            // Spotlight intensity
            float theta = dot(directionLight, normalize(-spotLights[i].direction));
            float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
            float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

            // Combine results
            vec3 ambient = spotLights[i].strengthAmbient * spotLights[i].ambient * attenuation * intensity * colorAmbient.rgb;
            vec3 diffuse = spotLights[i].strengthDiffuse * spotLights[i].diffuse * lambertFactor * attenuation * intensity * colorDiffuse.rgb;
            vec3 specular = spotLights[i].strengthSpecular * spotLights[i].specular * specularFactor * attenuation * intensity * colorSpecular.rgb;

            result += ambient + diffuse + specular;
        }
    }
    return result;
}
