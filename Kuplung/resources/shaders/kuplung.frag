uniform mat4 fs_MMatrix;
uniform sampler2D fs_sampler;
uniform float fs_alpha;
uniform float fs_refraction;
uniform float fs_shininess;

uniform vec3 fs_lightPosition;
uniform vec3 fs_lightDirection;
uniform float fs_specularStrength;
uniform float fs_ambientStrength;

uniform vec3 fs_ambientColor;
uniform vec3 fs_diffuseColor;
uniform vec3 fs_specularColor;

uniform vec3 fs_cameraPosition;
uniform float fs_screenResX, fs_screenResY;

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal;

out vec4 fragColor;

// https://github.com/planetspace/Overlord/blob/master/Overlord/Rendering/Shaders/Default.frag

void main(void) {
    vec4 texturedColor = texture(fs_sampler, fs_textureCoord);
    vec3 processedColor = texturedColor.rgb;

    // Ambient
    float ambientStrength = fs_ambientStrength;//0.1f;
    vec3 ambient = ambientStrength * fs_ambientColor; // fs_diffuseColor;

    // Diffuse
    vec3 norm = normalize(fs_vertexNormal);
    vec3 lightDir = normalize(fs_lightPosition - fs_vertexPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * fs_diffuseColor;

    // Specular
    float specularStrength = fs_specularStrength;//0.5f; // attenuation
    vec3 viewDir = normalize(fs_cameraPosition - fs_vertexPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * fs_specularColor;
//    specular = specularStrength * vec3(spec) * vec3(spec) * pow(max(0.0, dot(reflect(-fs_lightDirection, reflectDir), viewDir)), fs_shininess);

    if (fs_refraction > 1.0) {
        // Refraction (Optical Density)
        vec3 refraction = normalize(refract(fs_vertexPosition, norm, fs_refraction));
        vec3 refractionColor = mix(texture(fs_sampler, fs_textureCoord + refraction.xy * 0.1), texturedColor, fs_alpha).rgb;
        vec2 pixelTexCoords = vec2(gl_FragCoord.x / fs_screenResX, gl_FragCoord.y / fs_screenResY);
        processedColor = (ambient + diffuse + specular) * texture(fs_sampler, pixelTexCoords + refraction.xy * 0.1).rgb;
    }
    else {
        processedColor = (ambient + diffuse + specular) * texturedColor.rgb;
    }

    // Final color
    fragColor = vec4(processedColor, fs_alpha);
}
