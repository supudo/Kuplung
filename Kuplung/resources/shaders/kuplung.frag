uniform mat4 fs_MMatrix;
uniform sampler2D fs_sampler;
uniform float fs_alpha;
uniform float fs_refraction;

uniform vec3 fs_lightPosition;
uniform vec3 fs_lightDirection;

uniform vec3 fs_ambientColor;
uniform vec3 fs_diffuseColor;
uniform vec3 fs_specularColor;

uniform vec3 fs_cameraPosition;

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal;

out vec4 fragColor;

// https://github.com/planetspace/Overlord/blob/master/Overlord/Rendering/Shaders/Default.frag

void main(void) {
//    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
//    fragColor = texture(fs_sampler, fs_textureCoord);
//
//    vec4 texelColor = texture(fs_sampler, fs_textureCoord);
//    fragColor = vec4(texelColor.rgb, texelColor.a * fs_alpha);

    // ---
//
//    vec3 norm = normalize(fs_vertexNormal);
//
//    vec3 fragPos = vec3(fs_MMatrix * vec4(fs_vertexPosition, 1.0f));
//    vec3 lightDirection = normalize(fs_lightPosition - fragPos);
//
//    float diff = max(dot(norm, lightDirection), 0.0);
//    vec3 diffuseColor = diff * fs_lightColor;
//
//    vec3 processedColor = diffuseColor * texture(fs_sampler, fs_textureCoord).rgb;
//    fragColor = vec4(processedColor, fs_alpha);

    // ---

    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * fs_ambientColor;//fs_diffuseColor;

    // Diffuse
    vec3 norm = normalize(fs_vertexNormal);
    vec3 lightDir = normalize(fs_lightPosition - fs_vertexPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * fs_diffuseColor;

    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(fs_cameraPosition - fs_vertexPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * fs_specularColor;//fs_diffuseColor;

    // Refraction (Optical Density)
    //vec3 refraction = normalize(Refract(v_vertexPosition, norm, 1.20)); // u_refraction
    //vec3 refractionColor = mix(texture2D(Texture, v_textureCoord + refraction.xy * 0.1), fragColor, u_alpha).rgb;

    // Final color
    vec3 processedColor = (ambient + diffuse + specular) * texture(fs_sampler, fs_textureCoord).rgb;
    //vec3 processedColor = (ambient + diffuse + specular) * texture(fs_sampler, vs_textureCoord + refraction.xy * 0.1).rgb;
    fragColor = vec4(processedColor, fs_alpha);
}
