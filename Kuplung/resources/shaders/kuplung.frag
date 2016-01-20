uniform mat4 fs_MMatrix;
uniform vec3 fs_cameraPosition;
uniform float fs_screenResX, fs_screenResY;
uniform float fs_alpha;
uniform vec3 fs_outlineColor;

struct ModelMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;

    float shininess;
    float refraction;

    sampler2D sampler_ambient;
    sampler2D sampler_diffuse;
    sampler2D sampler_specular;
    sampler2D sampler_specularExp;
    sampler2D sampler_dissolve;

    bool has_texture_ambient;
    bool has_texture_diffuse;
    bool has_texture_specular;
    bool has_texture_specularExp;
    bool has_texture_dissolve;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float strengthAmbient;
    float strengthDiffuse;
    float strengthSpecular;

    vec3 position;
    vec3 direction;
};

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal;
in float fs_isBorder;

out vec4 fragColor;

uniform Light directionalLight[1];
uniform ModelMaterial material;

// https://github.com/planetspace/Overlord/blob/master/Overlord/Rendering/Shaders/Default.frag

vec3 calculateAmbient() {
    if (material.has_texture_ambient) {
        vec4 texturedColor_Ambient = texture(material.sampler_ambient, fs_textureCoord);
        return directionalLight[0].strengthAmbient * directionalLight[0].ambient * texturedColor_Ambient.rgb;
    }
    else
        return directionalLight[0].strengthAmbient * directionalLight[0].ambient * material.ambient;
}

vec3 calculateDiffuse(vec3 normalDirection, vec3 lightDirection) {
    float lambertFactor = max(dot(lightDirection, normalDirection), 0.0);
    if (material.has_texture_diffuse) {
        vec4 texturedColor_Diffuse = texture(material.sampler_diffuse, fs_textureCoord);
        return directionalLight[0].strengthDiffuse * lambertFactor * directionalLight[0].diffuse * texturedColor_Diffuse.rgb;
    }
    else
        return directionalLight[0].strengthDiffuse * lambertFactor * directionalLight[0].diffuse * material.diffuse;
}

vec3 calculateSpecular(vec3 normalDirection, vec3 viewDirection) {
    vec3 reflectDirection = reflect(-directionalLight[0].direction, normalDirection);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), directionalLight[0].strengthSpecular);
    vec3 specular = directionalLight[0].strengthSpecular * spec * directionalLight[0].specular;
    if (material.has_texture_specular) {
        vec4 texturedColor_Specular = texture(material.sampler_specular, fs_textureCoord);
        return specular * texturedColor_Specular.rgb;
    }
    else
        return specular * material.specular;
}

vec3 calculateRefraction(vec3 normalDirection, vec4 texturedColor_Diffuse) {
    vec3 refraction_normal = normalize(refract(fs_vertexPosition, normalDirection, material.refraction));
    vec3 refractionColor = mix(texture(material.sampler_diffuse, fs_textureCoord + refraction_normal.xy * 0.1), texturedColor_Diffuse, fs_alpha).rgb;
    vec2 pixelTexCoords = vec2(gl_FragCoord.x / fs_screenResX, gl_FragCoord.y / fs_screenResY);
    vec3 refraction = texture(material.sampler_diffuse, pixelTexCoords + refraction_normal.xy * 0.1).rgb;
    return refraction;
}

void main(void) {
    if (fs_isBorder > 0.0) {
        fragColor = vec4(fs_outlineColor, 1.0);
    }
    else {
        vec4 texturedColor_Diffuse = texture(material.sampler_diffuse, fs_textureCoord);
        vec3 processedColor_Diffuse = material.has_texture_diffuse ? texturedColor_Diffuse.rgb : material.diffuse;

        // misc
        vec3 normalDirection = normalize(fs_vertexNormal);
        vec3 lightDirection = normalize(directionalLight[0].position - fs_vertexPosition);
        vec3 viewDirection = normalize(fs_cameraPosition - fs_vertexPosition);

        // Ambient
        vec3 ambient = calculateAmbient();

        // Diffuse
        vec3 diffuse = calculateDiffuse(normalDirection, lightDirection);

        // Specular
        vec3 specular = calculateSpecular(normalDirection, viewDirection);

        // Refraction
        if (material.refraction > 1.0) {
            vec3 refraction = calculateRefraction(normalDirection, texturedColor_Diffuse);
            processedColor_Diffuse = (material.emission + ambient + diffuse + specular) * refraction;
        }
        else
            processedColor_Diffuse = (material.emission + ambient + diffuse + specular) * processedColor_Diffuse;

        // final color
        fragColor = vec4(processedColor_Diffuse, fs_alpha);
    }
}
